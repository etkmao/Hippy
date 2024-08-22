<template>
  <div id="shadow-demo">

    <div
      class="no-offset-shadow-demo-cube-android"
    >

      <div
        class="div-demo-3"
        :showsVerticalScrollIndicator="false"
      >
        <div class="display-flex flex-column">
          <p class="text-block">
            A
          </p>
          <p class="text-block">
            B
          </p>
          <p class="text-block">
            C
          </p>
          <p class="text-block">
            D
          </p>
          <p class="text-block">
            E
          </p>
        </div>
      </div>

    </div>

  </div>
</template>
<script>
import Vue from 'vue';

export default {
  data() {
    return {
      Platform: Vue.Native.Platform,
    };
  },
  mounted() {
    this.Platform = Vue.Native.Platform;
  },
};
</script>
<style scoped>
  #shadow-demo {
    flex: 1;
  }

  /**
   * P.S. Android shadow size is based on the container size
   * and a solid shadow background exists.
   * You should make content fully cover the solid shadow background
   * whose size is determined by container size, shadowRadius and shadowOffset,
   * so Android container with shadow(170*170) should be bigger than content(160*160),
   * and set some offset(e.g. top or left) for content to cover shadow background.
   *
   * boxShadow consists attrs as follow：boxShadowOpacity，boxShadowRadius，boxShadowColor，
   * boxShadowOffsetX，boxShadowOffsetY，boxShadowSpread(iOS only).
   *
   * 注意: Android的阴影大小根据容器体积大小来决定，通过在容器上对矩形view做阴影来实现，阴影会有一个实体的背景色。
   * 因此必须用content内容去遮挡住阴影的实体背景，该背景由容器大小、阴影圆角和阴影偏移共同决定。
   * Android带有阴影的Container容器大小(170*170)必须大于Content内容的大小(160*160)，
   * 同时要将Content内容做一定的偏移（如top或者left）来遮盖阴影背景。
   *
   * boxShadow支持以下属性：box-shadow-opacity，box-shadow-radius，box-shadow-color，
   * box-shadow-offset, box-shadow-offset-x，box-shadow-offset-y，box-shadow-spread(仅iOS).
   *
   * if you use border-radius, Android container & content both should be set.
   * 如果设置了border-radius，在Android上必须同时在Container容器和Content内容同时设置该属性。
   */
  #shadow-demo .no-offset-shadow-demo-cube-android {
    position: absolute;
    left: 50px;
    top: 50px;
    width: 170px;
    height: 270px;
    box-shadow-opacity: 0.6;
    box-shadow-radius: 5;
    box-shadow-color: #098a29;
    /* container & content should both set radius */
    /* 容器和内容都要设置border-radius */
    border-radius: 5px;
    padding-left: 5px;
    padding-top: 10px;
    padding-right: 5px;
    padding-bottom: 10px;
  }

  .div-demo-3 {
    /* width: 150px; */
    overflow-y: scroll;
    /* margin: 10px; */
    /* height: 320px; */
    border-color: #0b1695;
    border-width: 1px;
    background-color: bisque;
  }
  .text-block {
    width: 100px;
    height: 100px;
    line-height: 100px;
    border-style: solid;
    border-width: 1px;
    border-color: #40b883;
    font-size: 80px;
    margin: 20px;
    color: #40b883;
    text-align: center;
  }
</style>
