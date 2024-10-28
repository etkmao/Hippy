<template>
  <div class="button-demo">
    <label class="button-label">按钮和状态绑定</label>
    <button
      :class="{ 'is-active': true, 'is-pressing': false }"
      class="button-demo-1"
      @click="clickView"
    >
      <span
        v-if="isClicked"
        class="button-text"
      >视图已经被点击了，再点一下恢复</span>
      <span
        v-else
        class="button-text"
      >视图尚未点击</span>
    </button>
    <button
      :class="{ 'is-active': true, 'is-pressing': false }"
      class="button-demo-2"
    >
      <span
        class="button-text"
      >这里是第2个视图</span>
    </button>
    <img
      v-show="isClicked"
      alt="demo1-image"
      src="https://user-images.githubusercontent.com/12878546/148737148-d0b227cb-69c8-4b21-bf92-739fb0c3f3aa.png"
      class="button-demo-1-image"
    >
  </div>
</template>

<script>
export default {
  data() {
    return {
      isClicked: false,
      isPressing: false,
    };
  },
  methods: {
    clickView() {
      this.isClicked = !this.isClicked;
    },
    clickView2() {
      console.log('clickView2');
    },
    // button touch event is supported after hippy-vue 2.6.2
    onTouchBtnStart(evt) {
      console.log('onBtnTouchDown', evt);
      evt.stopPropagation();
    },
    // button touch event is supported after hippy-vue 2.6.2
    onTouchBtnMove(evt) {
      console.log('onBtnTouchMove', evt);
      evt.stopPropagation();
      console.log(evt);
    },
    // button touch event is supported after hippy-vue 2.6.2
    onTouchBtnEnd(evt) {
      console.log('onBtnTouchEnd', evt);
      evt.stopPropagation();
      console.log(evt);
    },
  },
};
</script>

<style scoped>
  .button-label {
    width: 220px;
    height: 50px;
    margin-top: 20px;
    text-align: center;
    line-height: 50px;
    margin-bottom: 20px;
  }
  .button-demo {
    display: flex;
    align-items: center;
    flex-direction: column;
  }

  .button-demo-1 {
    left: 20px;
    top: 100px;
    height: 64px;
    width: 240px;
    border-style: solid;
    border-color: #40b883;
    border-width: 2px;
    border-radius: 10px;
  }

  .button-demo-2 {
    left: 150px;
    top: 36px;
    height: 64px;
    width: 240px;
    border-style: solid;
    border-color: #103355;
    border-width: 2px;
    border-radius: 10px;
    background-color: #40b883;
  }

  .button-demo-1 .button-text {
    line-height: 56px;
    text-align: center;
  }

  .button-demo-1-image {
    width: 216px;
    height: 58px;
    background-color: #40b883;
    margin-top: 20px;
  }

  /*.button-demo-1.is-active {*/
  /*  color: white;*/
  /*  background-color: cornflowerblue;*/
  /*}*/

  /*.button-demo-1.is-pressing {*/
  /*  color: white;*/
  /*  background-color: blue;*/
  /*}*/
</style>
