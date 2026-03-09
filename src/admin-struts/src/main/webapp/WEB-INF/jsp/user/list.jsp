<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ taglib prefix="s" uri="/struts-tags" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<jsp:include page="/WEB-INF/jsp/common/header.jsp">
    <jsp:param name="title" value="ユーザー一覧"/>
</jsp:include>

<h2 class="page-title">ユーザー一覧</h2>

<table class="data-table">
    <thead>
        <tr>
            <th>ID</th>
            <th>ログインID</th>
            <th>ユーザー名</th>
            <th>種別</th>
            <th>作成日</th>
            <th>操作</th>
        </tr>
    </thead>
    <tbody>
        <s:iterator value="users" var="user">
            <tr>
                <td><s:property value="#user.userId"/></td>
                <td><s:property value="#user.loginId"/></td>
                <td><s:property value="#user.userName"/></td>
                <td><s:property value="#user.userType"/></td>
                <td><s:property value="#user.createdAt"/></td>
                <td>
                    <a href="${pageContext.request.contextPath}/admin/feature-flags?userId=<s:property value='#user.userId'/>"
                       class="btn btn-primary btn-sm">フラグ管理</a>
                </td>
            </tr>
        </s:iterator>
    </tbody>
</table>

<jsp:include page="/WEB-INF/jsp/common/footer.jsp"/>
