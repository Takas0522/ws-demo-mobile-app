<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ taglib prefix="s" uri="/struts-tags" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<jsp:include page="/WEB-INF/jsp/common/header.jsp">
    <jsp:param name="title" value="商品編集"/>
</jsp:include>

<h2 class="page-title">商品編集</h2>

<c:if test="${param.successMessage != null}">
    <div class="alert alert-success">
        <c:out value="${param.successMessage}"/>
    </div>
</c:if>

<s:if test="hasActionErrors()">
    <div class="alert alert-danger">
        <s:actionerror/>
    </div>
</s:if>

<div class="form-container">
    <s:form action="product-update" method="post" namespace="/admin" id="productForm">
        <s:hidden name="productId" value="%{product.productId}"/>

        <div class="form-group">
            <label for="productName">商品名 <span style="color: #dc3545;">*</span></label>
            <s:textfield name="productName" id="productName" value="%{product.productName}"
                         cssClass="form-control" maxlength="100"/>
            <s:fielderror fieldName="productName" cssClass="alert alert-danger"/>
        </div>

        <div class="form-group">
            <label for="unitPrice">単価（円）<span style="color: #dc3545;">*</span></label>
            <s:textfield name="unitPrice" id="unitPrice" value="%{product.unitPrice}"
                         cssClass="form-control" type="number" min="1"/>
            <s:fielderror fieldName="unitPrice" cssClass="alert alert-danger"/>
        </div>

        <div class="form-group">
            <label for="description">説明</label>
            <s:textarea name="description" id="description" value="%{product.description}"
                        cssClass="form-control" maxlength="500"/>
            <s:fielderror fieldName="description" cssClass="alert alert-danger"/>
        </div>

        <div class="form-group">
            <label for="imageUrl">画像URL</label>
            <s:textfield name="imageUrl" id="imageUrl" value="%{product.imageUrl}"
                         cssClass="form-control"/>
        </div>

        <div>
            <s:submit value="更新" cssClass="btn btn-primary" onclick="return confirmUpdate();"/>
            <a href="${pageContext.request.contextPath}/admin/products" class="btn btn-secondary">戻る</a>
        </div>
    </s:form>
</div>

<jsp:include page="/WEB-INF/jsp/common/footer.jsp"/>
