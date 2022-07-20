'use strict';

var log = console.log

// enum
const kGrindSize = 20
var eDirection = {
    Up:     0,
    Right:  1,
    Down:   2,
    Left:   3,
}
var eGridType = {
    Space:  0,
    Snake:   1,
    Apple:  2,
}
var eGridStyle = {
    Space: {
        "grid-space": true,
    },
    SnakeHead: {
        "grid-snake-head": true,
    },
    SnakeBody: {
        "grid-snake-body": true,
    },
    SnakeDead: {
        "grid-snake-dead": true,
    },
    Apple: {
        "grid-apple": true,
    },
}
var eGrid = {
    Space: {
        type: eGridType.Space,
        style: eGridStyle.Space,
    },
    SnakeHead: {
        type: eGridType.Snake,
        style: eGridStyle.SnakeHead,
    },
    SnakeBody: {
        type: eGridType.Snake,
        style: eGridStyle.SnakeBody,
    },
    SnakeDead: {
        type: eGridType.Snake,
        style: eGridStyle.SnakeDead,
    },
    Apple: {
        type: eGridType.Apple,
        style: eGridStyle.Apple,
    },
}

var Position = function(x, y) {
    return {
        x, y
    }
}

var snake = {
    direction: eDirection.Up,
    head: Position(0, 17),
    body: [
        Position(0, 18),
        Position(0, 19),
    ],
}

var app = new Vue({
    el: "#app",
    data: {
        version: 'v0.0.1',
        grids: [kGrindSize][kGrindSize],
        score: 0,
        moveSpeed: 1000, // 走一格使用的时间ms, 越小越快
        started: false,
        level: 1,
        isGameOver: false,
        timer: null,
    },

    computed: {
        getLevel() {
            if (this.score < 1) {
                return 1
            }
            return Math.floor(Math.log2(this.score))+1
        }
    },

    methods: {
        initSnake() {
            // head
            this.grids[snake.head.x][snake.head.y] = eGrid.SnakeHead
            // body
            for (let item of snake.body) {
                const x = item.x
                const y = item.y
                this.grids[x][y] = eGrid.SnakeBody
            }
            // apple
            this.produceApple()
        },

        redrawSnake(oldTail) {
            if (oldTail != null) {
                this.updateAGrid(oldTail, eGrid.Space)
            }
            for (let bodyItem of snake.body) {
                this.updateAGrid(bodyItem, eGrid.SnakeBody)
            }
            this.updateAGrid(snake.head, eGrid.SnakeHead)
        },

        makeSnakeDead() {
            this.isGameOver = true
            this.updateAGrid(snake.head, eGrid.SnakeDead)
        },

        produceApple() {
            let emptyPos = this.getRandomEmptyPos()
            this.updateAGrid(emptyPos, eGrid.Apple)
        },

        getRandomEmptyPos() {
            while (1) {
                let x = this.getRandomInt()
                let y = this.getRandomInt()
                if (this.grids[x][y].type === eGridType.Space) {
                    return new Position(x, y)
                }
            }
            
        },

        getRandomInt() {
            return Math.floor(Math.random() * 20);
        },

        checkBoundary(pos) {
            // return false
            if (pos.x < 0 || pos.x > 19) {
                return false
            }
            if (pos.y < 0 || pos.y > 19) {
                return false
            }

            return true
        },

        updateAGrid(pos, gridType) {
            this.grids[pos.x][pos.y] = gridType
            Vue.set(this.grids, pos.x, this.grids[pos.x])
        },

        initGridBoard() {
            let size = kGrindSize
            this.grids = new Array()
            for (let i = 0; i < size; i++) {
                this.grids[i] = new Array()
                for (let j = 0; j < size; j++) {
                    this.grids[i][j] = eGrid.Space
                }
            }
            log("initGridBoard done")
        },

        isApple(pos) {
            return this.grids[pos.x][pos.y].type === eGridType.Apple
        },

        moveSnake() {
            if (!this.started || this.isGameOver) {
                return false
            }

            let newHead = new Position(snake.head.x, snake.head.y)
            switch (snake.direction) {
                case eDirection.Up:
                    newHead.y--
                    // update head
                    break;
                case eDirection.Right:
                    newHead.x++
                    // update head
                    break;
                case eDirection.Down:
                    newHead.y++
                    // update head
                    break;
                case eDirection.Left:
                    newHead.x--
                    // update head
                    break;
            
                default:
                    return false;
            }

            let oldHead = new Position(snake.head.x, snake.head.y)
            if (!this.checkBoundary(newHead)) {
                this.makeSnakeDead()
                // alert("Game Over! your score is " + this.score)
                return false
            }

            // update model
            snake.head = newHead
            snake.body.unshift(oldHead)
            // update view
            let oldTail = null
            if (this.isApple(newHead)) {
                this.score++
                this.produceApple()
            } else {
                oldTail = snake.body.pop()
            }
            this.redrawSnake(oldTail)

            return true
        },

        moveSnakeWithTimer() {
            if (this.moveSnake()) {
                let newSpeed = this.moveSpeed * Math.pow(0.8, this.getLevel-1)
                this.timer = window.setTimeout(this.moveSnakeWithTimer, newSpeed)
            }
        },

        clearTimer() {
            if (this.timer == null) {
                return
            }
            window.clearTimeout(this.timer)
            this.timer = null
        },

        initTimer() {
        },
        initEvent() {
            document.addEventListener("keydown", this.handleKeyDown)
        },

        handleAppClick() {
            log("click")
        },
        handleKeyDown(event) {
            if (event.key == " ") {
                if (this.isGameOver) {
                    location.reload();
                }
                if (this.started) {
                    this.clearTimer()
                }
                this.started = !this.started
                this.moveSnakeWithTimer()
                return
            } 

            this.clearTimer()

            if (event.key == "ArrowUp") {
                snake.direction = eDirection.Up
            } else if (event.key == "ArrowRight") {
                snake.direction = eDirection.Right
            } else if (event.key == "ArrowDown") {
                snake.direction = eDirection.Down
            } else if (event.key == "ArrowLeft") {
                snake.direction = eDirection.Left
            } else {
                log("unsupported key")
                return
            }
            // 加速
            this.moveSnakeWithTimer()
        }
    },

    mounted() {
        this.initGridBoard()
        this.initSnake()
        this.initTimer()
        this.initEvent()
    }
})

