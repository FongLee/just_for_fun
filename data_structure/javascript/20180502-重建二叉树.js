function TreeNode(x) {
    this.val = x;
    this.left = null;
    this.right = null;
}

/**
 * 输入某二叉树的前序遍历和中序遍历的结果，请重建出该二叉树。
 * 假设输入的前序遍历和中序遍历的结果中都不含重复的数字。
 * 例如输入前序遍历序列{1,2,4,7,3,5,6,8}和
 * 中序遍历序列{4,7,2,1,5,3,8,6}，则重建二叉树并返回。
 * @param  {array} 
 * @param  {array}
 * @return {TreeNode}
 */
function reConstructBinaryTree(pre, vin) {
    debugger;
    if (!pre || !vin || pre.length < 1 || vin.length < 1) {
        return;
    }

    var rootVal = pre[0];

    var rootNode = new TreeNode(rootVal);
    var leftLen = vin.indexOf(rootVal);
    var rightLen = vin.length - leftLen - 1;

    var leftPre = pre.slice(1, leftLen + 1);
    var rightPre = pre.slice(leftLen + 1, pre.length);
    var leftVin = vin.slice(0, leftLen);
    var rightVin = vin.slice(leftLen + 1, vin.length);

    var rootLeftNode = reConstructBinaryTree(leftPre, leftVin);
    var rootrightNode = reConstructBinaryTree(rightPre, rightVin);
    
    rootNode.left = rootLeftNode;
    rootNode.right = rootrightNode;
    
    return rootNode;


}