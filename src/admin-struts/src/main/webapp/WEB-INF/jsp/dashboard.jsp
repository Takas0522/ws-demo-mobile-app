<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<jsp:include page="/WEB-INF/jsp/common/header.jsp">
    <jsp:param name="title" value="ダッシュボード"/>
</jsp:include>

<h2 class="page-title">ダッシュボード</h2>

<div class="card-grid">
    <a href="${pageContext.request.contextPath}/admin/products" class="card">
        <h2>商品管理</h2>
        <p>商品の一覧表示・編集を行います</p>
    </a>

    <a href="${pageContext.request.contextPath}/admin/users" class="card">
        <h2>ユーザー管理</h2>
        <p>登録ユーザーの一覧を確認します</p>
    </a>

    <a href="${pageContext.request.contextPath}/admin/feature-flags" class="card">
        <h2>フラグ管理</h2>
        <p>ユーザー別フィーチャーフラグの管理を行います</p>
    </a>
</div>

<jsp:include page="/WEB-INF/jsp/common/footer.jsp"/>
