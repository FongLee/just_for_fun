/**
 * 请实现一个函数，将一个字符串中的空格替换成“%20”。
 * 例如，当字符串为We Are Happy.则经过替换之后的
 * 字符串为We%20Are%20Happy。
 * @param  {string}
 * @return {string}
 */
function replaceSpace(str) {
    debugger;

    var retStr = "";
    for (var i = 0; i < str.length; i++) {
        if (str[i] === ' ') {
            retStr = retStr.concat("%20")
        } else {
            retStr = retStr.concat(str[i]);
        }
    }

    return retStr;

}