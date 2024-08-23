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
          <span style="font-size: 19px; color: white">小肥ཽε</span>
          <span>:</span>
          <span>等到的时候散户手上的票全部跌了80%，你能涨的上去吗</span>
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
      img1: 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABgAAAAYCAMAAADXqc3KAAAANlBMVEUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC3dmhyAAAAEXRSTlMA9QlZEMPc2Mmmj2VkLEJ4Rsx+pEgAAAChSURBVCjPjVLtEsMgCDOAdbbaNu//sttVPes+zvGD8wgQCLp/TORbUGMAQtQ3UBeSAMlF7/GV9Cmb5eTJ9R7H1t4bOqLE3rN2UCvvwpLfarhILfDjJL6WRKaXfzxc84nxAgLzCGSGiwKwsZUB8hPorZwUV1s1cnGKw+yAOrnI+7hatNIybl9Q3OkBfzopCw6SmDVJJiJ+yD451OS0/TNM7QnuAAbvCG0TSAAAAABJRU5ErkJggg==',
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
