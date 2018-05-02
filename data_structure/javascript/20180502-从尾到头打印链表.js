/*function ListNode(x){
    this.val = x;
    this.next = null;
}*/
// 输入一个链表，从尾到头打印链表每个节点的值。
function printListFromTailToHead(head) {
    var queue = [];
    var curr = head;
    while (curr) {
        queue.push(curr.val);
        curr = curr.next;
    }

    var ret = [];
    while (queue.length) {
        ret.push(queue.pop());
    }

    return ret;

}