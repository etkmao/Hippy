<template>
   <div
    class="p-demo"
    :style="{color: topColor}"
    >
    <div>

      <div class="p-demo-content">
        <p
          :style="{ backgroundColor: '#40b883', marginBottom: 10, paddingHorizontal: 10, paddingVertical: 5 }"
        >
          <span style="font-size: 19px; color: white">园丁W</span>
          <span></span>
          <span>:</span>
          <span>股息在4%以上，交银股价</span>
          <img
            :style="{ width: 24, height: 24 }"
            :src="img1"
          >
        </p>

      </div>

    </div>
  </div>
</template>

<script>
import Vue from 'vue';

export default {
  data() {
    return {
      topColor: 'grey',
      Platform: Vue.Native.Platform,
      textShadowIndex: 0,
      isClicked: false,
      isPressing: false,
      labelTouchStatus: '',
      textShadow: {
        textShadowOffset: {
          x: 1,
          y: 1,
        },
        // support declare textShadowOffsetX & textShadowOffsetY separately
        // textShadowOffsetX: 1,
        // textShadowOffsetY: 1,
        textShadowRadius: 3,
        textShadowColor: 'grey',
      },
      textMode: {
        numberOfLines: 2,
        ellipsizeMode: 'tail',
      },
      img1: 'data:image/png;base64,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',
      img2: `data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAMAAADXqc3KAAAANlBMVEUAAAAAAAAAAAAAAAAAAAAAAAAA
        AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC3dmhyAAAAEnRSTlMA/QpX7WQU2m27pi3Ej9KEQXaD5HhjAAAAqklEQVQoz41
        SWxLDIAh0RcFXTHL/yzZSO01LMpP9WJEVUNA9gfdXTioCSKE/kQQTQmf/ArRYva+xAcuPP37seFII2L7FN4BmXdHzlEPIpDHiZ0A7eIViPc
        w2QwqipkvMSdNEFBUE1bmMNOyE7FyFaIkAP4jHhhG80lvgkzBODTKpwhRMcexuR7fXzcp08UDq6GRbootp4oRtO3NNpd4NKtnR9hB6oaefw
        eIFQU0EfnGDRoQAAAAASUVORK5CYII=`,
      img3: 'https://user-images.githubusercontent.com/12878546/148736255-7193f89e-9caf-49c0-86b0-548209506bd6.gif',
      longText: 'The 58-letter name Llanfairpwllgwyngyllgogerychwyrndrobwllllantysiliogogogoch is '
        + 'the name of a town on Anglesey, an island of Wales.',
      breakStrategy: 'simple',
    };
  },
  methods: {
    changeTextShadow() {
      this.textShadow = {
        textShadowOffsetX: this.textShadowIndex % 2 === 1 ? 10 : 1,
        textShadowOffsetY: 1,
        textShadowRadius: 3,
        textShadowColor: this.textShadowIndex % 2 === 1 ? 'red' : 'grey',
      };
      this.textShadowIndex += 1;
    },
    changeTextColor() {
      this.topColor = this.textShadowIndex % 2 === 1 ? 'red' : 'grey',
      this.textShadowIndex += 1;
    },
    // text/span/label/p/a element touch event is supported after hippy-vue 2.6.2
    onTouchTextStart(evt) {
      this.labelTouchStatus = 'touch start';
      console.log('onTextTouchDown', evt);
      evt.stopPropagation();
    },
    // text/span/label/p/a element touch event is supported after hippy-vue 2.6.2
    onTouchTextMove(evt) {
      this.labelTouchStatus = 'touch move';
      console.log('onTextTouchMove', evt);
      evt.stopPropagation();
      console.log(evt);
    },
    // text/span/label/p/a element touch event is supported after hippy-vue 2.6.2
    onTouchTextEnd(evt) {
      this.labelTouchStatus = 'touch end';
      console.log('onTextTouchEnd', evt);
      evt.stopPropagation();
      console.log(evt);
    },
    incrementLine() {
      if (this.textMode.numberOfLines < 6) {
        this.textMode.numberOfLines += 1;
      }
    },
    decrementLine() {
      if (this.textMode.numberOfLines > 1) {
        this.textMode.numberOfLines -= 1;
      }
    },
    changeMode(mode) {
      this.textMode.ellipsizeMode = mode;
    },
    changeBreakStrategy(strategy) {
      this.breakStrategy = strategy;
    },
  },
};
</script>

<style scoped>
.p-demo {
  margin: 7px;
  overflow-y: scroll;
  flex: 1;
  flex-direction: column;
}

.p-demo .p-demo-content {
  margin: 20px;
}

.p-demo .p-demo-content-status {
  margin-left: 20px;
  margin-right: 20px;
  margin-bottom: 10px;
}

.p-demo .p-demo-1 {
  color: #f44837;
}
.p-demo .p-demo-2 {
  font-size: 30px;
}
.p-demo .p-demo-3 {
  font-weight: bold;
}

.p-demo .p-demo-4 {
  text-decoration-line: underline;
  text-decoration-style: dotted;
}

.p-demo .p-demo-5 {
  text-decoration-line: line-through;
  text-decoration-color: red;
}

.p-demo .p-demo-6 {
  color: #0052d9;
  font-family: TTTGB;
  font-size: 32px;
}

.p-demo .p-demo-7 {
  /*text-shadow-offset: 1px 1px; !* not support declared separately *!*/
  /*text-shadow-radius: 3;*/
  /*text-shadow-color: grey;*/
}

.p-demo .p-demo-8 {
  letter-spacing: -1px;
}

.p-demo .p-demo-9 {
  letter-spacing: 5px;
}

.p-demo .button-bar {
  flex-direction: row;
}

.p-demo .button {
  width: 100px;
  margin: 2px;
  background-color: #eee;
  border-style: solid;
  border-color: black;
  border-width: 1px;
  align-items: center;
  flex-shrink: 1;
}
</style>
