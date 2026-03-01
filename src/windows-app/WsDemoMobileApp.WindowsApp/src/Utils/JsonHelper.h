#pragma once

#include <string>
#include <expected>
#include <optional>
#include <nlohmann/json.hpp>
#include "Models/ApiError.h"

namespace ws::utils
{

// BFF APIレスポンスの共通解析結果
template <typename T>
using ApiResult = std::expected<T, ws::models::ApiError>;

// BFFレスポンスの "data" フィールドを解析
template <typename T>
[[nodiscard]] inline ApiResult<T> ParseApiResponse(const std::string& responseBody)
{
	try
	{
		auto json = nlohmann::json::parse(responseBody);

		if (json.contains("error") && !json.at("error").is_null())
		{
			auto error = json.at("error").get<ws::models::ApiError>();
			return std::unexpected(std::move(error));
		}

		if (json.contains("data"))
		{
			return json.at("data").get<T>();
		}

		return json.get<T>();
	}
	catch (const nlohmann::json::exception& e)
	{
		return std::unexpected(ws::models::ApiError{
			"JSON_PARSE_ERROR",
			std::string("JSONの解析に失敗しました: ") + e.what()
		});
	}
}

// BFFレスポンスのエラーチェックのみ
[[nodiscard]] inline std::optional<ws::models::ApiError> ParseApiError(const std::string& responseBody)
{
	try
	{
		auto json = nlohmann::json::parse(responseBody);

		if (json.contains("error") && !json.at("error").is_null())
		{
			return json.at("error").get<ws::models::ApiError>();
		}

		return std::nullopt;
	}
	catch (const nlohmann::json::exception&)
	{
		return ws::models::ApiError{"JSON_PARSE_ERROR", "JSONの解析に失敗しました"};
	}
}

} // namespace ws::utils
