WRMCB=function(e){var c=console;if(c&&c.log&&c.error){c.log('Error running batched script.');c.error(e);}}
;
try {
/* module-key = 'com.atlassian.confluence.plugins.confluence-request-access-plugin:confluence-request-access-plugin-resources', location = '/js/request-access-page.js' */
require(["ajs","jquery","confluence/legacy","confluence/meta"],function(a,c,d,b){a.toInit(function(){var f=b.get("page-id");var e=b.get("remote-user");var j=c(".request-access-container");var i=c(".request-access-container button");var h=i.data("access-type");var g=c("#invite-to-edit-draft").length;if(g){f=c("#invite-to-edit-draft").data("draft-id")}if(j.length){c("#breadcrumbs").hide();c("#title-text.with-breadcrumbs").hide();if(i.length){a.trigger("analyticsEvent",{name:"confluence.request.access.plugin.request.access.to.page.view",data:{pageId:f,requestAccessUser:e,accessType:h}})}}i.prop("disabled",false);i.removeAttr("aria-disabled");i.click(function(){a.trigger("analyticsEvent",{name:"confluence.request.access.plugin.request.access.to.page",data:{pageId:f,requestAccessUser:e,accessType:h}});i.attr("aria-disabled","true");var k=c(d.Request.Access.loading({}));i.replaceWith(k);c.ajax({url:d.getContextPath()+"/rest/access/latest/page/restriction/"+f+"/request/"+h,type:"POST",contentType:"application/json; charset=utf-8",success:function(l){a.flag({type:"success",title:"Request sent",body:a.format("We\u0027\u0027ve asked {0} to give you access. You\u0027\u0027ll get an email once the request is approved",d.Request.Access.usernameLink({user:l}))})},error:function(l,m){a.flag({type:"error",title:"Access request unsuccessful",body:"Your request for access has not been sent. Contact your space admin."})},complete:function(){k.remove();d.Binder.userHover()}})})})});
}catch(e){WRMCB(e)};
;
try {
/* module-key = 'com.atlassian.confluence.plugins.confluence-request-access-plugin:confluence-request-access-plugin-resources', location = '/js/request-edit-access-dialog.js' */
require(["ajs","jquery","confluence/legacy","confluence/meta"],function(a,c,d,b){a.toInit(function(){var e=WRM.data.claim("com.atlassian.confluence.plugins.confluence-request-access-plugin:confluence-request-access-plugin-resources.mail-server-configured");var l=c("#system-content-items");var n=c("#content-metadata-page-restrictions.restricted").length!==0;var h=b.get("page-id");var i=b.get("remote-user");if(!l.length||!n||c("#editPageLink").length||!j()){return}var k=c(d.Request.Access.loading());var f=a.InlineDialog(l,"requestAccessDialog",g,m());function g(p,o,q){p.css({padding:"20px"}).html(d.Request.Access.dialog({canRequestAccess:e&&i}));p.on("click","#request-access-dialog-button",function(t){t.stopPropagation();var r=p.find(".actions-result");r.replaceWith(k);a.trigger("analyticsEvent",{name:"confluence.request.access.plugin.request.access.to.page",data:{pageId:h,requestAccessUser:i,accessType:"edit"}});var s="";c.ajax({url:d.getContextPath()+"/rest/access/latest/page/restriction/"+h+"/request/edit",type:"POST",contentType:"application/json; charset=utf-8",data:i,success:function(u){a.flag({type:"success",title:"Request sent",body:a.format("We\u0027\u0027ve asked {0} to give you access. You\u0027\u0027ll get an email once the request is approved",d.Request.Access.usernameLink({user:u}))})},error:function(u){if(u.status==412){s="Access was granted, but there is not a mail server configured so the notification could not be sent."}else{if(u.status==502){s="Access was granted, but an unexpected error happened while sending the notification."}else{s="Sorry, there was an unexpected error while granting access."}}a.flag({type:"error",title:"Access request unsuccessful",body:s})},complete:function(){k.remove();f.hide()}})});p.on("click",".aui-button.cancel",function(r){f.hide()});q();return false}function m(){return{offsetY:2,offsetX:0,width:350,hideDelay:null,noBind:true,hideCallback:function(){setTimeout(f.hide(),5000)}}}function j(){var o=window.location.search.match(/[\?&]requestEditAccess=/);return !!(o&&o.length)}f.show()})});
}catch(e){WRMCB(e)};
;
try {
/* module-key = 'com.atlassian.confluence.plugins.confluence-request-access-plugin:confluence-request-access-plugin-resources', location = '/js/grant-access.js' */
require(["ajs","jquery","confluence/legacy","confluence/meta"],function(a,c,d,b){a.toInit(function(){var e=b.get("page-id");var i=b.get("remote-user");var n=q("username");var h=q("accessType");var g=q("userFullName");var k=c("#system-content-items");var r=c("#content-metadata-page-restrictions.restricted").length!==0;var j=c("#rte-button-restrictions");var l=m()&&j.length&&q("grantAccess")&&h;var p=k.length&&r&&q("grantAccess")&&h;if(!p&&!l){return}if(l){k=j;e=b.get("draft-id")}var f=c(d.Request.Access.loading());var o=a.InlineDialog(k,"grantAccessDialog",function(t,s,u){t.css({padding:"20px"}).html(d.Grant.Access.dialog({requestAccessUsername:n,requestAccessUserFullName:g,requestAccessType:h,contentType:b.get("content-type")}));t.on("click",".aui-button.grant-access",function(x){x.stopPropagation();var w=t.find(".actions-result");w.replaceWith(f);a.trigger("analyticsEvent",{name:"confluence.request.access.plugin.grant.access.to.page",data:{pageId:e,grantAccessUser:i,requestAccessUser:n,accessType:h}});var v="";c.ajax({url:d.getContextPath()+"/rest/access/latest/page/restriction/"+e+"/grant/"+h,type:"POST",contentType:"application/json; charset=utf-8",data:n,success:function(z,A,y){if(y.status==202){v="Access was already granted to the user."}else{v="Access was granted, a notification to the user will be sent."}a.flag({type:"success",title:"Access request granted",body:v})},error:function(y){if(y.status==412){v="Access was granted, but there is not a mail server configured so the notification could not be sent."}else{if(y.status==502){v="Access was granted, but an unexpected error happened while sending the notification."}else{v="Sorry, there was an unexpected error while granting access."}}a.flag({type:"error",title:"Access request error",body:v})},complete:function(y){o.hide()}})});t.on("click",".aui-button.deny-access",function(v){a.trigger("analyticsEvent",{name:"confluence.request.access.plugin.deny.access.to.page",data:{pageId:e,grantAccessUser:i,requestAccessUser:n,accessType:h}});o.hide()});u();return false},{offsetY:2,offsetX:0,width:350,hideDelay:null,noBind:true,hideCallback:function(){setTimeout(o.hide(),5000)}});o.show();function q(s){s=s.replace(/[\[]/,"\\[").replace(/[\]]/,"\\]");var u=new RegExp("[\\?&]"+s+"=([^&#]*)"),t=u.exec(location.search);return t==null?"":decodeURIComponent(t[1].replace(/\+/g," "))}function m(){return a.Rte&&a.Rte.getEditor()&&(!!a.$("#editpageform").length||!!a.$("#createpageform").length)}})});
}catch(e){WRMCB(e)};
;
try {
/* module-key = 'com.atlassian.confluence.plugins.confluence-request-access-plugin:confluence-request-access-plugin-resources', location = '/templates/soy/request-access.soy' */
// This file was automatically generated from request-access.soy.
// Please don't edit this file by hand.

/**
 * @fileoverview Templates in namespace Confluence.Request.Access.
 */

if (typeof Confluence == 'undefined') { var Confluence = {}; }
if (typeof Confluence.Request == 'undefined') { Confluence.Request = {}; }
if (typeof Confluence.Request.Access == 'undefined') { Confluence.Request.Access = {}; }


Confluence.Request.Access.usernameLink = function(opt_data, opt_ignored) {
  return '<a href="' + soy.$$escapeHtml("") + '/display/~' + soy.$$escapeUri(opt_data.user.name) + '" class="url fn confluence-userlink" title data-username="' + soy.$$escapeHtml(opt_data.user.name) + '">' + soy.$$escapeHtml(opt_data.user.fullName) + '</a>';
};
if (goog.DEBUG) {
  Confluence.Request.Access.usernameLink.soyTemplateName = 'Confluence.Request.Access.usernameLink';
}


Confluence.Request.Access.loading = function(opt_data, opt_ignored) {
  return '<span id="request-access-loading" class=\'aui-icon aui-icon-wait\'>' + soy.$$escapeHtml('Loading, please wait') + '</span>"';
};
if (goog.DEBUG) {
  Confluence.Request.Access.loading.soyTemplateName = 'Confluence.Request.Access.loading';
}


Confluence.Request.Access.dialog = function(opt_data, opt_ignored) {
  return '<div class="request-access-dialog"><h2 class="grant-access-title">' + soy.$$escapeHtml('You don\x27t have permission to edit') + '</h2>' + ((opt_data.canRequestAccess) ? '<p class="grant-access-message">' + soy.$$escapeHtml('Hit request access and we\x27ll find someone who can give you access.') + '</p><div class="actions-result"><button id="request-access-dialog-button" class="aui-button">' + soy.$$escapeHtml('Request access') + '</button><button class="aui-button aui-button-link cancel">' + soy.$$escapeHtml('Cancel') + '</button><div>' : '<p class="grant-access-message">' + soy.$$escapeHtml('A space admin or the person who shared this page may be able to give you access.') + '</p><div class="actions-result"><button class="aui-button aui-button-link cancel">' + soy.$$escapeHtml('Close') + '</button><div>') + '</div>';
};
if (goog.DEBUG) {
  Confluence.Request.Access.dialog.soyTemplateName = 'Confluence.Request.Access.dialog';
}

}catch(e){WRMCB(e)};
;
try {
/* module-key = 'com.atlassian.confluence.plugins.confluence-request-access-plugin:confluence-request-access-plugin-resources', location = '/templates/soy/grant-access.soy' */
// This file was automatically generated from grant-access.soy.
// Please don't edit this file by hand.

/**
 * @fileoverview Templates in namespace Confluence.Grant.Access.
 */

if (typeof Confluence == 'undefined') { var Confluence = {}; }
if (typeof Confluence.Grant == 'undefined') { Confluence.Grant = {}; }
if (typeof Confluence.Grant.Access == 'undefined') { Confluence.Grant.Access = {}; }


Confluence.Grant.Access.dialog = function(opt_data, opt_ignored) {
  var output = '<div class="grant-access-dialog">';
  var usernameLink__soy4 = '' + Confluence.Grant.Access.usernameLink({username: opt_data.requestAccessUsername, userFullName: opt_data.requestAccessUserFullName});
  var requestAccessDescription__soy8 = '' + ((opt_data.requestAccessType == 'edit') ? (opt_data.contentType == 'blogpost') ? soy.$$filterNoAutoescape(AJS.format('{0} wants to \x3cstrong\x3eedit\x3c/strong\x3e this blog post.',usernameLink__soy4)) : soy.$$filterNoAutoescape(AJS.format('{0} wants to \x3cstrong\x3eedit\x3c/strong\x3e this page.',usernameLink__soy4)) : (opt_data.contentType == 'blogpost') ? soy.$$filterNoAutoescape(AJS.format('{0} wants to \x3cstrong\x3eview\x3c/strong\x3e this blog post.',usernameLink__soy4)) : soy.$$filterNoAutoescape(AJS.format('{0} wants to \x3cstrong\x3eview\x3c/strong\x3e this page.',usernameLink__soy4)));
  output += '<h2 class="title grant-access-title">' + soy.$$escapeHtml('Access request') + '</h2><p class="grant-access-message">' + soy.$$filterNoAutoescape(requestAccessDescription__soy8) + '</p><div class="actions-result"><button class="aui-button grant-access">' + soy.$$escapeHtml('Grant access') + '</button><button class="aui-button aui-button-link deny-access">' + soy.$$escapeHtml('Deny') + '</button><div></div>';
  return output;
};
if (goog.DEBUG) {
  Confluence.Grant.Access.dialog.soyTemplateName = 'Confluence.Grant.Access.dialog';
}


Confluence.Grant.Access.usernameLink = function(opt_data, opt_ignored) {
  return '<a href="' + soy.$$escapeHtml("") + '/display/~' + soy.$$escapeHtml(opt_data.username) + '" class="url fn" title data-username="' + soy.$$escapeHtml(opt_data.username) + '"><strong>' + soy.$$escapeHtml(opt_data.userFullName) + '</strong> (' + soy.$$escapeHtml(opt_data.username) + ')</a>';
};
if (goog.DEBUG) {
  Confluence.Grant.Access.usernameLink.soyTemplateName = 'Confluence.Grant.Access.usernameLink';
}

}catch(e){WRMCB(e)};