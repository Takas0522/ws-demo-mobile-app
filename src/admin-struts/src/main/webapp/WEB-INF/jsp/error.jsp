<%@ page contentType="text/html; charset=UTF-8" %>
<%@ taglib prefix="s" uri="/struts-tags" %>
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>エラー</title>
    <link rel="stylesheet" href="${pageContext.request.contextPath}/css/style.css"/>
</head>
<body>
    <div class="error-container">
        <h1>エラーが発生しました</h1>
        <p><s:property value="exception.message" default="予期しないエラーが発生しました。"/></p>
        <a href="${pageContext.request.contextPath}/admin/dashboard">ダッシュボードに戻る</a>
    </div>
</body>
</html>
