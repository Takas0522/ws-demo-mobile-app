#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <bcrypt.h>

#include <string>
#include <expected>
#include <optional>
#include <vector>
#include <chrono>
#include <nlohmann/json.hpp>

namespace ws::utils
{

// Base64URL encode/decode (no padding)
inline std::string Base64UrlEncode(const std::vector<uint8_t>& data)
{
	static const char* table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	std::string result;
	result.reserve((data.size() + 2) / 3 * 4);

	for (size_t i = 0; i < data.size(); i += 3)
	{
		uint32_t n = static_cast<uint32_t>(data[i]) << 16;
		if (i + 1 < data.size()) n |= static_cast<uint32_t>(data[i + 1]) << 8;
		if (i + 2 < data.size()) n |= static_cast<uint32_t>(data[i + 2]);

		result += table[(n >> 18) & 0x3F];
		result += table[(n >> 12) & 0x3F];
		if (i + 1 < data.size()) result += table[(n >> 6) & 0x3F];
		if (i + 2 < data.size()) result += table[n & 0x3F];
	}

	// Convert to URL-safe base64 (no padding)
	for (auto& c : result)
	{
		if (c == '+') c = '-';
		else if (c == '/') c = '_';
	}
	return result;
}

inline std::string Base64UrlEncode(const std::string& str)
{
	return Base64UrlEncode(std::vector<uint8_t>(str.begin(), str.end()));
}

inline std::vector<uint8_t> Base64UrlDecode(const std::string& input)
{
	std::string b64 = input;
	for (auto& c : b64)
	{
		if (c == '-') c = '+';
		else if (c == '_') c = '/';
	}
	// Add padding
	while (b64.size() % 4 != 0) b64 += '=';

	static const int lookup[] = {
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,62,-1,-1,-1,63,
		52,53,54,55,56,57,58,59,60,61,-1,-1,-1,-1,-1,-1,
		-1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,
		15,16,17,18,19,20,21,22,23,24,25,-1,-1,-1,-1,-1,
		-1,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,
		41,42,43,44,45,46,47,48,49,50,51,-1,-1,-1,-1,-1
	};

	std::vector<uint8_t> result;
	result.reserve(b64.size() / 4 * 3);

	for (size_t i = 0; i < b64.size(); i += 4)
	{
		int n = 0;
		int pad = 0;
		for (int j = 0; j < 4; j++)
		{
			if (b64[i + j] == '=')
			{
				pad++;
				n <<= 6;
			}
			else
			{
				int val = lookup[static_cast<unsigned char>(b64[i + j])];
				if (val < 0) return {};
				n = (n << 6) | val;
			}
		}
		result.push_back(static_cast<uint8_t>((n >> 16) & 0xFF));
		if (pad < 2) result.push_back(static_cast<uint8_t>((n >> 8) & 0xFF));
		if (pad < 1) result.push_back(static_cast<uint8_t>(n & 0xFF));
	}
	return result;
}

// HMAC-SHA256 using Windows BCrypt API
inline std::vector<uint8_t> HmacSha256(const std::string& key, const std::string& message)
{
	BCRYPT_ALG_HANDLE hAlg = nullptr;
	BCRYPT_HASH_HANDLE hHash = nullptr;
	std::vector<uint8_t> result(32);
	DWORD hashObjSize = 0;
	DWORD cbData = 0;

	if (BCryptOpenAlgorithmProvider(&hAlg, BCRYPT_SHA256_ALGORITHM, nullptr, BCRYPT_ALG_HANDLE_HMAC_FLAG) != 0)
		return {};

	BCryptGetProperty(hAlg, BCRYPT_OBJECT_LENGTH, reinterpret_cast<PUCHAR>(&hashObjSize), sizeof(DWORD), &cbData, 0);
	std::vector<uint8_t> hashObj(hashObjSize);

	if (BCryptCreateHash(hAlg, &hHash, hashObj.data(), hashObjSize,
		reinterpret_cast<PUCHAR>(const_cast<char*>(key.data())),
		static_cast<ULONG>(key.size()), 0) != 0)
	{
		BCryptCloseAlgorithmProvider(hAlg, 0);
		return {};
	}

	BCryptHashData(hHash, reinterpret_cast<PUCHAR>(const_cast<char*>(message.data())),
		static_cast<ULONG>(message.size()), 0);
	BCryptFinishHash(hHash, result.data(), static_cast<ULONG>(result.size()), 0);

	BCryptDestroyHash(hHash);
	BCryptCloseAlgorithmProvider(hAlg, 0);
	return result;
}

// Create JWT token
inline std::string CreateJwt(
	int64_t userId,
	const std::string& loginId,
	const std::string& userType,
	const std::string& secret,
	int expirationSeconds)
{
	auto now = std::chrono::system_clock::now();
	auto iat = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
	auto exp = iat + expirationSeconds;

	nlohmann::json header = {
		{"alg", "HS256"},
		{"typ", "JWT"}
	};

	nlohmann::json payload = {
		{"sub", std::to_string(userId)},
		{"loginId", loginId},
		{"userType", userType},
		{"iat", iat},
		{"exp", exp}
	};

	std::string headerEncoded = Base64UrlEncode(header.dump());
	std::string payloadEncoded = Base64UrlEncode(payload.dump());
	std::string signingInput = headerEncoded + "." + payloadEncoded;

	auto signature = HmacSha256(secret, signingInput);
	std::string signatureEncoded = Base64UrlEncode(signature);

	return signingInput + "." + signatureEncoded;
}

struct JwtPayload
{
	int64_t userId = 0;
	std::string loginId;
	std::string userType;
	int64_t iat = 0;
	int64_t exp = 0;
};

// Verify and decode JWT token
inline std::expected<JwtPayload, std::string> VerifyJwt(
	const std::string& token,
	const std::string& secret)
{
	// Split token into parts
	size_t dot1 = token.find('.');
	if (dot1 == std::string::npos) return std::unexpected("Invalid token format");
	size_t dot2 = token.find('.', dot1 + 1);
	if (dot2 == std::string::npos) return std::unexpected("Invalid token format");

	std::string headerEncoded = token.substr(0, dot1);
	std::string payloadEncoded = token.substr(dot1 + 1, dot2 - dot1 - 1);
	std::string signatureEncoded = token.substr(dot2 + 1);

	// Verify signature
	std::string signingInput = headerEncoded + "." + payloadEncoded;
	auto expectedSig = HmacSha256(secret, signingInput);
	std::string expectedSigEncoded = Base64UrlEncode(expectedSig);

	if (signatureEncoded != expectedSigEncoded)
		return std::unexpected("Invalid signature");

	// Decode payload
	auto payloadBytes = Base64UrlDecode(payloadEncoded);
	if (payloadBytes.empty())
		return std::unexpected("Failed to decode payload");

	try
	{
		std::string payloadStr(payloadBytes.begin(), payloadBytes.end());
		auto json = nlohmann::json::parse(payloadStr);

		JwtPayload result;
		result.userId = std::stoll(json.at("sub").get<std::string>());
		json.at("loginId").get_to(result.loginId);
		json.at("userType").get_to(result.userType);
		json.at("iat").get_to(result.iat);
		json.at("exp").get_to(result.exp);

		// Check expiration
		auto now = std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();
		if (now >= result.exp)
			return std::unexpected("Token expired");

		return result;
	}
	catch (const std::exception& e)
	{
		return std::unexpected(std::string("Failed to parse payload: ") + e.what());
	}
}

} // namespace ws::utils
