<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ taglib prefix="s" uri="/struts-tags" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<jsp:include page="/WEB-INF/jsp/common/header.jsp">
    <jsp:param name="title" value="商品一覧"/>
</jsp:include>

<h2 class="page-title">商品一覧</h2>

<table class="data-table">
    <thead>
        <tr>
            <th>ID</th>
            <th>商品名</th>
            <th>単価（円）</th>
            <th>説明</th>
            <th>操作</th>
        </tr>
    </thead>
    <tbody>
        <s:iterator value="products" var="product">
            <tr>
                <td><s:property value="#product.productId"/></td>
                <td><s:property value="#product.productName"/></td>
                <td><s:property value="#product.unitPrice"/></td>
                <td><s:property value="#product.description" default="-"/></td>
                <td>
                    <a href="${pageContext.request.contextPath}/admin/product-edit?productId=<s:property value='#product.productId'/>"
                       class="btn btn-primary btn-sm">編集</a>
                </td>
            </tr>
        </s:iterator>
    </tbody>
</table>

<jsp:include page="/WEB-INF/jsp/common/footer.jsp"/>
