<script setup lang="ts">
import { onActivated, onMounted, proxyRefs, reactive, ref } from 'vue';

// 棋盘的宽高
const kWidth = 30;
const kHeight = 30;
const kInitSpeed = 300; // 初始化速度，多少ms移动一次

let score = ref(0)
let level = ref(1)
let gameOver = ref(false)

enum EState {
  Empty,
  Body,
  Food,
}

const board = reactive([])
for (let i = 0; i < kHeight; i++) {
  board[i] = []
  for (let j = 0; j < kWidth; j++) {
    board[i][j] = EState.Empty
  }
}

type Pos = {
  x: number,
  y: number,
};
class Direction {
  static Left: Pos = { x: 0, y: -1, }
  static Down: Pos = { x: 1, y: 0, }
  static Right: Pos = { x: 0, y: 1, }
  static Up: Pos = { x: -1, y: 0, }
}

class Snake {
  private bodies: Pos[] // 身体各个块
  direction: Pos // 当前方向
  private food: Pos // 食物位置
  private moveDelayMs: number // 多少ms移动一次位置
  timer: number;

  constructor() {
    this.bodies = [{ x: 0, y: 0 }]
    board[0][0] = EState.Body
    this.direction = Direction.Right
    this.food = this.genEmptyPos()
    this.moveDelayMs = kInitSpeed
    gameOver.value = false
  }

  move(): void {
    const head: Pos = {
      x: this.bodies[0].x,
      y: this.bodies[0].y,
    };
    // 移动
    head.x += this.direction.x
    head.y += this.direction.y
    // 检测边界碰撞
    if (head.x < 0 || head.x >= kWidth ||
      head.y < 0 || head.y >= kHeight
    ) {
      gameOver.value = true
      return
    }
    // 检测是否撞到自己
    if (this.isPointInBodies(head)) {
      gameOver.value = true
      return
    }

    // 更新头部
    this.bodies.unshift(head)
    board[head.x][head.y] = EState.Body

    if (!this.isPosFood(head)) {
      // 没吃到食物则移除尾巴
      let tail = this.bodies.pop()
      board[tail.x][tail.y] = EState.Empty
    } else {
      // 吃到食物则刷新食物位置
      this.food = this.genEmptyPos()
      // 加分
      this.addScore()
    }
  }

  addScore() {
    score.value++
    if (score.value % 2 == 0) {
      level.value++
      this.setTimer()
    }
  }

  setTimer() {
    clearInterval(this.timer)
    this.timer = setInterval(() => {
      this.move()
      if (gameOver.value) {
        clearInterval(this.timer)
      }
    }, this.moveDelayMs - (level.value - 1) * 10)
  }

  clearTimer() {
    // console.log("this.timer", this.timer);
    if (this.timer > 0) {
      clearInterval(this.timer)
      this.timer = 0;
    } else {
      this.setTimer()
    }
  }

  isPointInBodies(p: Pos): boolean {
    return this.bodies.findIndex(
      body => body.x === p.x &&
        body.y === p.y
    ) >= 0
  }

  isPosFood(p: Pos): boolean {
    return p.x == this.food.x && p.y == this.food.y
  }

  isEmptyPos(pos: Pos): boolean {
    return !this.isPointInBodies(pos)
  }

  genEmptyPos(): Pos {
    // FIXME 优化：这里可以把所有空闲位置缓存起来再随机取
    while (true) {
      let pos = this.genRandomPos()
      if (this.isEmptyPos(pos)) {
        board[pos.x][pos.y] = EState.Food
        return pos
      }
    }
  }

  genRandomPos(): Pos {
    return {
      x: Math.floor(Math.random() * kWidth),
      y: Math.floor(Math.random() * kHeight),
    }
  }
}

const snake = new Snake()

onMounted(() => {
  snake.setTimer()
  document.addEventListener('keydown', function (event) {
    if (gameOver.value) {
      return
    }
    // console.log("keydown:", event.code);
    let newDir: Pos;
    if (event.code === "ArrowUp") {
      newDir = Direction.Up
    } else if (event.code === "ArrowRight") {
      newDir = Direction.Right
    } else if (event.code === "ArrowDown") {
      newDir = Direction.Down
    } else if (event.code === "ArrowLeft") {
      newDir = Direction.Left
    } else if (event.code === "Space") {
      snake.clearTimer()
      return
    } else {
      return
    }
    // 不允许反方向
    if (newDir.x * snake.direction.x + newDir.y * snake.direction.y == -1) {
      return
    }
    snake.direction = newDir
    snake.move()
  });
})
</script>

<template>
  <div id="main">
    <div id="stage">
      <div id="snake">
        <template v-for="(item, i) in board">
          <div v-for="(state, j) in item"
            :class="{ isEmpty: state === EState.Empty, isBody: state === EState.Body, isFood: state === EState.Food }"
            class="board-grid2">
          </div>
        </template>
      </div>
    </div>
    <div id="score-panel">
      <span>SCORE: {{ score }}</span>
      <span v-show="gameOver" class="gameOver">游戏结束</span>
      <span>LEVEL: {{ level }}</span>
    </div>
  </div>
</template>

<style scoped>
/* @bg-color: #b7d4a8 */
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

.gameOver {
  color: red;
}

#main {
  width: 360px;
  height: 420px;
  background-color: #b7d4a8;
  margin: 100px auto;
  border: 10px solid black;
  border-radius: 40px;
  font: bold 20px "Courier";
  display: flex;
  flex-flow: column;
  align-items: center;
  justify-content: space-around;
}

#stage {
  width: 304px;
  height: 304px;
  border: 2px solid black;
  position: relative;
}

#score-panel {
  width: 300px;
  display: flex;
  justify-content: space-between;
}

#snake {
  display: flex;
  flex-flow: row wrap;
}

.snake-body,
.isBody {
  width: 10px;
  height: 10px;
  background-color: black;
  border: 1px solid #b7d4a8;
  /* position: absolute; */
}

.board-grid,
.isEmpty {
  width: 10px;
  height: 10px;
  background-color: #b7d4a8;
}


.food,
.isFood {
  width: 10px;
  height: 10px;
  background-color: red;
  border-radius: 2px;
  /* position: absolute; */
  /* left: 60px; */
  /* top: 60px; */
  /* display: flex; */
  /* flex-flow: row wrap; */
  /* justify-content: space-between; */
  /* align-content: space-between; */
}
</style>