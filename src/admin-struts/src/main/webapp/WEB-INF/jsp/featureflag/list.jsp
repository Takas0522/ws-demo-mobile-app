<%@ page contentType="text/html; charset=UTF-8" pageEncoding="UTF-8" %>
<%@ taglib prefix="s" uri="/struts-tags" %>
<%@ taglib prefix="c" uri="http://java.sun.com/jsp/jstl/core" %>
<jsp:include page="/WEB-INF/jsp/common/header.jsp">
    <jsp:param name="title" value="フラグ管理"/>
</jsp:include>

<h2 class="page-title">フィーチャーフラグ管理</h2>

<c:if test="${param.successMessage != null}">
    <div class="alert alert-success">
        <c:out value="${param.successMessage}"/>
    </div>
</c:if>

<%-- ユーザー選択フォーム --%>
<div class="form-container mb-1">
    <s:form action="feature-flags" method="get" namespace="/admin">
        <div class="form-group">
            <label for="userId">ユーザーを選択</label>
            <s:select name="userId" id="userId" list="users"
                      listKey="userId" listValue="%{userName + ' (' + loginId + ')'}"
                      headerKey="" headerValue="-- ユーザーを選択 --"
                      cssClass="form-control"/>
        </div>
        <s:submit value="表示" cssClass="btn btn-primary"/>
    </s:form>
</div>

<%-- フラグ一覧（ユーザー選択時のみ表示） --%>
<s:if test="targetUser != null">
    <h3 class="mt-1"><s:property value="targetUser.userName"/> のフラグ設定</h3>

    <table class="data-table mt-1">
        <thead>
            <tr>
                <th>フラグ名</th>
                <th>フラグキー</th>
                <th>状態</th>
                <th>操作</th>
            </tr>
        </thead>
        <tbody>
            <s:iterator value="featureFlags" var="flag">
                <tr>
                    <td><s:property value="#flag.flagName"/></td>
                    <td><s:property value="#flag.flagKey"/></td>
                    <td>
                        <s:if test="#flag.isEnabled">
                            <span class="flag-enabled">有効</span>
                        </s:if>
                        <s:else>
                            <span class="flag-disabled">無効</span>
                        </s:else>
                    </td>
                    <td>
                        <s:form action="feature-flag-update" method="post" namespace="/admin"
                                theme="simple" style="display: inline;">
                            <s:hidden name="userId" value="%{targetUser.userId}"/>
                            <s:hidden name="flagKey" value="%{#flag.flagKey}"/>
                            <s:if test="#flag.isEnabled">
                                <s:hidden name="enabled" value="false"/>
                                <s:submit value="無効にする" cssClass="btn btn-danger btn-sm btn-toggle"/>
                            </s:if>
                            <s:else>
                                <s:hidden name="enabled" value="true"/>
                                <s:submit value="有効にする" cssClass="btn btn-success btn-sm btn-toggle"/>
                            </s:else>
                        </s:form>
                    </td>
                </tr>
            </s:iterator>
        </tbody>
    </table>
</s:if>

<jsp:include page="/WEB-INF/jsp/common/footer.jsp"/>
