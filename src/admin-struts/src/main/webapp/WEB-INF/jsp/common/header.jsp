<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ taglib prefix="s" uri="/struts-tags" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<!DOCTYPE html>
<html lang="ja">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>${param.title} - 管理画面</title>
    <link rel="stylesheet" href="${pageContext.request.contextPath}/css/style.css"/>
</head>
<body>
    <header class="site-header">
        <div class="header-container">
            <h1 class="site-title">管理画面</h1>
            <nav class="main-nav">
                <a href="${pageContext.request.contextPath}/admin/dashboard">ダッシュボード</a>
                <a href="${pageContext.request.contextPath}/admin/products">商品管理</a>
                <a href="${pageContext.request.contextPath}/admin/users">ユーザー管理</a>
                <a href="${pageContext.request.contextPath}/admin/feature-flags">フラグ管理</a>
                <a href="${pageContext.request.contextPath}/admin/logout" class="logout-link">ログアウト</a>
            </nav>
            <c:if test="${sessionScope.loginUserName != null}">
                <span class="user-info">ログイン中: <c:out value="${sessionScope.loginUserName}"/></span>
            </c:if>
        </div>
    </header>
    <main class="main-content">
