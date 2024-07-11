
// document.addEventListener('DOMContentLoaded', function() {
//     // 这里的代码会在DOM完全加载并解析完成后执行
//     console.log('DOM is fully loaded and parsed.');
// });
function func() {
document.addEventListener('onload', function() { /* 等待DOM加载完成 */
    console.log("wtf");
    let board = document.getElementById('game-board'); /* 获取棋盘容器 */
    const boardSize = 15; /* 棋盘大小为15x15 */

    // 动态生成棋盘
    for (let i = 0; i < boardSize; i++) {
        let row = document.createElement('div'); /* 创建行容器 */
        for (let j = 0; j < boardSize; j++) {
            let cell = document.createElement('div'); /* 创建单元格 */
            cell.className = 'cell'; /* 给单元格添加CSS类 */
            cell.textContent = "wtf";
            cell.addEventListener('click', function() { /* 为单元格添加点击事件 */
                // 这里可以添加放置棋子的逻辑
                console.log(`Cell ${i}-${j} clicked`); /* 控制台输出点击位置 */
            });
            row.appendChild(cell); /* 将单元格添加到行容器 */
        }
        board.appendChild(row); /* 将行容器添加到棋盘容器 */
    }
});
}