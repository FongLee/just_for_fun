
/**
 * 二维数组中的查找
 * 在一个二维数组中，每一行都按照从左到右递增的顺序排序，
 * 每一列都按照从上到下递增的顺序排序。
 * 请完成一个函数，输入这样的一个二维数组和一个整数，
 * 判断数组中是否含有该整数。
 * @param {number}
 * @param {array}
 */
function Find(target, array) {
    var row = 0;
    var column = array[0].length;
    while (row < array.length && column >= 0) {
        if (target === array[row][column]) {
            return true;
        } else if (target > array[row][column]) {
            row++;
        } else {
            column--;
        }
    }
    return false;
}