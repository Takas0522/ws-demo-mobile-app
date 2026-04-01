<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ taglib prefix="s" uri="/struts-tags" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<jsp:include page="/WEB-INF/jsp/common/header.jsp">
    <jsp:param name="title" value="価格履歴"/>
</jsp:include>

<h2 class="page-title">価格履歴</h2>

<s:if test="hasActionErrors()">
    <div class="alert alert-danger">
        <s:actionerror/>
    </div>
</s:if>

<div class="product-info-card mb-1">
    <table class="data-table">
        <thead>
            <tr>
                <th>商品ID</th>
                <th>商品名</th>
                <th>現在の単価（円）</th>
            </tr>
        </thead>
        <tbody>
            <tr>
                <td><s:property value="product.productId"/></td>
                <td><s:property value="product.productName"/></td>
                <td><s:property value="product.unitPrice"/></td>
            </tr>
        </tbody>
    </table>
</div>

<p class="mb-1" style="color: #666; font-size: 0.9rem;">
    全 <s:property value="totalItems"/> 件の価格変更履歴
    <s:if test="totalPages > 0">
        （<s:property value="currentPage"/> / <s:property value="totalPages"/> ページ）
    </s:if>
</p>

<s:if test="priceHistory != null && priceHistory.size() > 0">
    <table class="data-table">
        <thead>
            <tr>
                <th>変更日時</th>
                <th>変更前単価（円）</th>
                <th>変更後単価（円）</th>
                <th>変更者</th>
                <th>変更理由</th>
            </tr>
        </thead>
        <tbody>
            <s:iterator value="priceHistory" var="h">
                <tr>
                    <td><s:property value="#h.changedAt"/></td>
                    <td><s:property value="#h.oldPrice"/></td>
                    <td><s:property value="#h.newPrice"/></td>
                    <td><s:property value="#h.changedBy.userName" default="不明"/></td>
                    <td><s:property value="#h.changeReason" default="-"/></td>
                </tr>
            </s:iterator>
        </tbody>
    </table>

    <s:if test="totalPages > 1">
        <div class="pagination mt-1">
            <s:if test="currentPage > 1">
                <a href="${pageContext.request.contextPath}/admin/product-price-history?productId=<s:property value='productId'/>&amp;page=<s:property value='currentPage - 1'/>"
                   class="btn btn-secondary btn-sm">&laquo; 前へ</a>
            </s:if>

            <span class="pagination-info">
                <s:property value="currentPage"/> / <s:property value="totalPages"/>
            </span>

            <s:if test="currentPage < totalPages">
                <a href="${pageContext.request.contextPath}/admin/product-price-history?productId=<s:property value='productId'/>&amp;page=<s:property value='currentPage + 1'/>"
                   class="btn btn-secondary btn-sm">次へ &raquo;</a>
            </s:if>
        </div>
    </s:if>
</s:if>
<s:else>
    <div class="alert alert-warning">
        価格変更履歴はありません。
    </div>
</s:else>

<div class="mt-1">
    <a href="${pageContext.request.contextPath}/admin/product-edit?productId=<s:property value='productId'/>"
       class="btn btn-primary">商品編集へ</a>
    <a href="${pageContext.request.contextPath}/admin/products"
       class="btn btn-secondary">商品一覧へ</a>
</div>

<jsp:include page="/WEB-INF/jsp/common/footer.jsp"/>
