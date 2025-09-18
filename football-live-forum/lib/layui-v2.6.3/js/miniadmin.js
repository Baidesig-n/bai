// lib/layui-v2.6.3/js/miniadmin.js
layui.define(['layer', 'element', 'jquery'], function (exports) {
    var layer = layui.layer,
        element = layui.element,
        $ = layui.jquery;

    var miniAdmin = {
        render: function (options) {
            // 模拟菜单加载
            $.getJSON(options.iniUrl, function (res) {
                console.log('菜单已加载', res);
                // layuimini 会自动处理菜单渲染
            });
            layer.ready(function () {
                layer.msg('欢迎使用 足球直播论坛', {time: 2000});
            });
        }
    };

    exports('miniAdmin', miniAdmin);
});