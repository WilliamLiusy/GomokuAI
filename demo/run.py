from flask import Flask, render_template, jsonify, request
from datetime import timedelta
import subprocess
import timeout_decorator
import time
import numpy as np
import sys

class AI:
    def __init__(self, path, id):
        self.path = path
        if path == 'human':
            self.human = 1
        else:
            self.human = 0
        self.id = id

    def send(self, message):
        value = str(message) + '\n'
        value = bytes(value, 'UTF-8')
        self.proc.stdin.write(value)
        self.proc.stdin.flush()

    def receive(self):
        return self.proc.stdout.readline().strip().decode()

    # @timeout_decorator.timeout(seconds = 5, use_signals = True)
    def init(self):
        if self.human == 0:
            self.proc = subprocess.Popen(self.path,
                                         stdin=subprocess.PIPE,
                                         stdout=subprocess.PIPE)
            self.send(self.id)
            self.name = self.receive()

    # @timeout_decorator.timeout(seconds = 5, use_signals = True)
    def action(self, a, b):
        if self.human == 1:
            value = sys.stdin.readline().strip().split(' ')
        else:
            self.send(str(a) + ' ' + str(b))
            value = self.receive().split(' ')
        return int(value[0]), int(value[1])


class Board:
    def __init__(self):
        self.board = -np.ones((15, 15), dtype=int)

    def show(self):
        for i in range(15):
            for j in range(15):
                if self.board[i][j] == -1:
                    sys.stdout.write('_ ')
                else:
                    sys.stdout.write(str(self.board[i][j]) + ' ')
            sys.stdout.write('\n')

    def action(self, side, turn, x, y):
        if turn == 2 and side == 1 and x == -1 and y == -1:
            self.board = np.where(self.board != -1, 1 - self.board, self.board)
        else:
            self.board[x][y] = side

    def full(self):
        return len(np.where(self.board == -1)[0]) == 0

    def check_win(self, side, turn, x, y):
        if x == -1 and y == -1:
            return 0
        if x < 0 or x >= 15 or y < 0 or y >= 15 or self.board[x][y] != -1:
            return -1

        # 8 Directions
        dx = [1, 1, -1, -1, 0, 0, 1, -1]
        dy = [1, -1, 1, -1, 1, -1, 0, 0]
        for xx in range(0, 15):
            for yy in range(0, 15):

                for i in range(8):
                    curx, cury = xx, yy
                    flg = True
                    for _ in range(5):
                        if curx < 0 or curx >= 15 or cury < 0 or cury >= 15:
                            flg = False
                            break

                        if self.board[curx][cury] != side and (curx != x or cury != y):
                            flg = False
                            break
                        curx, cury = curx + dx[i], cury + dy[i]
                        # print(_, curx, cury, self.board[curx][cury], side)
                    if flg:
                        return 1

        return 0


def try_init(ai0, ai1):
    try:
        ai0.init()
    except:
        sys.stderr.write('Time out: ai0 timeout in init function\n')
    try:
        ai1.init()
    except:
        sys.stderr.write('Time out: ai1 timeout in init function\n')


board = Board()
player_side = 0
player = None
MyAI = None
turn = 0
a, b = -1, -1


app = Flask(__name__)
app.config['SEND_FILE_MAX_AGE_DEFAULT'] = timedelta(seconds=1)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/init<data>', methods = ['GET'])
def INIT(data):
    data_ = int(data)
    global player_side, player, MyAI
    player_side = data_
    if player_side == 0:
        player = AI('human', 0)
        MyAI = AI('judge/MyAI', 1)
        try_init(player, MyAI)
    else:
        player = AI('human', 1)
        MyAI = AI('judge/MyAI', 0)
        try_init(MyAI, player)
    return 'init success'


## 有个问题：AI先手我写的是要等点击
## 解决：开始游戏时如果玩家是后手，开始游戏的按钮定向成-1，-1过来。
## 如果要写用户换手，搞个换手按钮定向成-1，-1过来
@app.route('/action/<x>+<y>', methods=['GET'])
def Move(x, y):
    x_ = int(x)
    y_ = int(y)
    global board, player, MyAI, turn, a, b
    turn += 1
    board.show()
    a, b = x_, y_
    if turn == 2 and player_side == 1 and a == -1 and b == -1:
        sys.stderr.write('player flips the board\n')
    else:
        sys.stderr.write('player take action: [' + str(a) + ' ' + str(b) + ']\n')
    ret = board.check_win(player_side, turn, a, b)
    board.action(player_side, turn, a, b)
    
    board.show()
    if ret == -1:
        return jsonify({'result': False})
    elif ret == 1:
        return jsonify({'result': True, 'Final': True, 'message': player_side})
    elif board.full():
        return jsonify({'result': True, 'Final': True, 'message': 2})

    a, b = MyAI.action(a, b)
    if turn == 2 and player_side == 0 and a == -1 and b == -1:
        sys.stderr.write('MyAI flips the board\n')
    else:
        sys.stderr.write('MyAI take action: [' + str(a) + ' ' + str(b) + ']\n')
    ret = board.check_win(1 - player_side, turn, a, b)
    board.action(1 - player_side, turn, a, b)
    board.show()
    if ret == -1:
        return jsonify({'result': False})
    elif ret == 1:
        return jsonify({'result': True, 'Final': True, 'message': 1 - player_side, 'x': a, 'y':b})
    elif board.full():
        return jsonify({'result': True, 'Final': True, 'message': 2})
        
    return jsonify({'result': True, 'Final': False, 'x': a, 'y':b})


@app.route('/Final', methods=['POST'])
def Final(winner):
    if winner == player_side:
        return "Congratulations! You Win!"
    else:
        return "You Lose."
        ## 放一个按钮重新开始

if __name__ == '__main__':
    # app.run()
    app.run(debug=True)