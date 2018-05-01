/**
 * 
 * 冒泡排序-稳定
 * 最好情况 O(n)
 * 最差情况 O(n^2)
 * 平均情况 O(n^2)
 * var arr=[3,44,38,5,47,15,36,26,27,2,46,4,19,50,48];
 * console.log(bubbleSort3(arr));
 * @param  {array}
 * @return {array}
 */
function bubbleSort(arr) {
    var len = arr.length;
    var flag = false;
    for (var i = len - 1; i >= 1; i--) {
        for (var j = 0; j < i; j++) {
            if (arr[j] > arr[i]) {
                var tmp = arr[i];
                arr[i] = arr[j];
                arr[j] = tmp;
                flag = true;
            }
        }
        if (!flag) {
            break;
        }
    }

    return arr;
}

/**
 * 选择排序
 * 最佳情况 O(n^2)
 * 最差情况 O(n^2)
 * 平均情况 O(n^2)
 * @param  {array}
 * @return {array}
 */
function selectSort(arr) {

    var len = arr.length;
    for (var i = 0; i < len - 1; i++) {
        var minIndex = i;
        for (var j = i + 1; j < len; j++) {
            if (arr[j] < arr[minIndex]) {
                minIndex = j;
            }

        }
        var tmp = arr[i];
        arr[i] = arr[minIndex];
        arr[minIndex] = tmp;
    }
    return arr;
}

/**
 * 插入排序
 * 最佳情况 O(n)
 * 最坏情况 O(n2)
 * 平均情况 O(n2)
 * @param  {array}
 * @return {array}
 */
function insertSort(arr) {
    var len = arr.length;

    for (var i = 1; i < len; i++) {
        var tmp = arr[i];
        for (var j = i - 1; j >= 0; j--) {
            if (arr[j] > tmp) {
                arr[j + 1] = arr[j];
            } else {
                break;
            }

        }
        arr[j + 1] = tmp;
    }
    return arr;
}

/**
 * 希尔排序
 * 最佳情况 O(nlog2 n)
 * 最坏情况 O(nlog2 n)
 * 平均情况 O(nlog n)
 * @param  {array}
 * @return {array}
 */
function shellSort(arr) {
    var len = arr.length;

    for (var d = Math.floor(len / 2); d >= 1; d = Math.floor(d / 2)) {
        for (var i = d; i < len; i++) {
            var tmp = arr[i];
            for (var j = i - d; j >= 0 && tmp < arr[j]; j = j - d) {
                arr[j + d] = arr[j];
            }
            arr[j + d] = tmp;
        }
    }
    return arr;
}

/**
 * 归并排序
 * 最佳情况 O(n)
 * 最差情况 O(nlogn)
 * 平均情况 O(nlogn)
 * @param  {array}
 * @return {array}
 */
function mergeSort(arr) {
    debugger;
    if (arr.length <= 1) {
        return arr;
    }

    var middle = Math.floor(arr.length / 2);
    var left = arr.slice(0, middle);
    var right = arr.slice(middle);


    return mergeTwo(mergeSort(left), mergeSort(right));

}

function mergeTwo(left, right) {
    var ret = [];

    if (!left && !right) {
        return [];
    }

    var i = 0;
    var j = 0;
    while (left.length && right.length) {
        if (left[i] < right[j]) {
            ret.push(left.shift());
            i++;
        } else {
            ret.push(right.shift());
            j++;
        }
    }

    return ret.concat(left, right);

}

/**
 * 快速排序
 * 最佳情况 O(nlogn)
 * 最差情况 O(n2)
 * 平均情况 O(nlogn)
 * @param  {array}
 * @return {array}
 */
function quickSort(arr) {
    if (arr.length <= 1) {
        return arr;
    }
    var pivotIndex = Math.floor(arr.length / 2);
    var pivot = arr.splice(pivotIndex, 1)[0];
    var left = [];
    var right = [];
    for (var i = 0; i < arr.length; i++) {
        if (arr[i] < pivot) {
            left.push(arr[i]);
        } else {
            right.push(arr[i]);
        }

    }

    return quickSort(left).concat(pivot, quickSort(right));


}

/**
 * 堆排序
 * 最佳情况 O(nlogn)
 * 最差情况 O(nlogn)
 * 平均情况 O(nlogn)   
 * @param  {array}
 * @return {array}
 */
function heapSort(arr) {
    if (arr.length < 2) {
        return arr;
    }
    var tmp;
    for (var i = Math.floor(arr.length / 2) - 1; i >= 0; i--) {
        heapify(arr, i, arr.length);
    }

    for (var i = arr.length - 1; i >= 1; i--) {
        tmp = arr[i];
        arr[i] = arr[0];
        arr[0] = tmp;
        heapify(arr, 0, i);
    }
    return arr;
}

function heapify(arr, i, len) {

    var left = 2 * i + 1;
    var right = 2 * i + 2;
    var largest = i;

    if (i >= len) {
        return;
    }

    if (left < len && arr[left] > arr[largest]) {
        largest = left;
    }

    if (right < len && arr[right] > arr[largest]) {
        largest = right;
    }

    if (largest !== i) {
        var tmp = arr[i];
        arr[i] = arr[largest];
        arr[largest] = tmp;
        heapify(arr, largest, len);
    }
}