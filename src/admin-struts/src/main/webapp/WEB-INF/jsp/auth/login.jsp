<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ taglib prefix="s" uri="/struts-tags" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<!DOCTYPE html>
<html lang="ja">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ログイン - 管理画面</title>
    <link rel="stylesheet" href="${pageContext.request.contextPath}/css/style.css"/>
</head>
<body>
    <div class="login-container">
        <div class="login-card">
            <h1>管理者ログイン</h1>

            <s:if test="hasActionErrors()">
                <div class="alert alert-danger">
                    <s:actionerror/>
                </div>
            </s:if>

            <s:form action="login-submit" method="post" namespace="/admin">
                <div class="form-group">
                    <label for="loginId">ログインID</label>
                    <s:textfield name="loginId" id="loginId" cssClass="form-control"
                                 placeholder="ログインIDを入力"/>
                    <s:fielderror fieldName="loginId" cssClass="alert alert-danger"/>
                </div>

                <div class="form-group">
                    <label for="password">パスワード</label>
                    <s:password name="password" id="password" cssClass="form-control"
                                placeholder="パスワードを入力"/>
                    <s:fielderror fieldName="password" cssClass="alert alert-danger"/>
                </div>

                <s:submit value="ログイン" cssClass="btn btn-primary" style="width: 100%;"/>
            </s:form>
        </div>
    </div>
</body>
</html>
