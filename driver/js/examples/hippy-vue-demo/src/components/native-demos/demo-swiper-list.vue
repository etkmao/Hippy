<template>
  <div id="demo-swiper-list">
    <div class="toolbar">
      <button
        class="toolbar-btn"
        @click="scrollToPrevPage"
      >
        <span>Prev Page</span>
      </button>
      <button
        class="toolbar-btn"
        @click="scrollToNextPage"
      >
        <span>Next Page</span>
      </button>
      <p class="toolbar-text">
        Current: {{ currentSlideNum + 1 }} / {{ slideCount }}
      </p>
    </div>

    <swiper
      id="swiper"
      ref="swiper"
      need-animation
      :current="currentSlide"
      @dropped="onDropped"
    >
      <swiper-slide
        v-for="slideIndex in slides"
        :key="slideIndex"
        class="swiper-slide"
      >
        <ul
          class="slide-list"
          :numberOfRows="listDataSource[slideIndex].length"
        >
          <pull-header
            :ref="`pullHeader-${slideIndex}`"
            class="ul-refresh"
            @pulling="(evt) => onHeaderPulling(evt, slideIndex)"
            @released="() => onHeaderReleased(slideIndex)"
          >
            <p class="ul-refresh-text">
              {{ headerRefreshText[slideIndex] }}
            </p>
          </pull-header>

          <li
            v-for="item in listDataSource[slideIndex]"
            :key="item.id"
            class="item-style"
          >
            <p class="item-title">
              {{ item.title }}
            </p>
            <p class="item-subtitle">
              {{ item.desc }}
            </p>
          </li>
        </ul>
      </swiper-slide>
    </swiper>

    <div id="swiper-dots">
      <div
        v-for="slideIndex in slides"
        :key="`dot-${slideIndex}`"
        class="dot"
        :class="{ hightlight: currentSlideNum === slideIndex }"
      />
    </div>
  </div>
</template>

<script>
function createMockItems(slideIndex, version = 1) {
  return (new Array(12)).fill(0).map((_, index) => ({
    id: `${slideIndex}-${version}-${index}`,
    title: `Slide ${slideIndex + 1} - Item ${index + 1}`,
    desc: `Refresh version ${version}`,
  }));
}

export default {
  data() {
    const slideCount = 3;
    const slides = (new Array(slideCount)).fill(0).map((_, i) => i);
    const listDataSource = slides.map((slideIndex) => createMockItems(slideIndex));
    const headerRefreshText = slides.map(() => 'Pull down to refresh');
    const refreshingFlags = slides.map(() => false);
    const refreshVersion = slides.map(() => 1);

    return {
      slideCount,
      slides,
      currentSlide: 0,
      currentSlideNum: 0,
      listDataSource,
      headerRefreshText,
      refreshingFlags,
      refreshVersion,
    };
  },
  mounted() {
    this.$maxSlideIndex = this.$refs.swiper.$el.childNodes.length - 1;

    this.slides.forEach((slideIndex) => {
      this.collapsePullHeader(slideIndex, 100);
    });
  },
  methods: {
    mockFetchData(slideIndex) {
      return new Promise((resolve) => {
        setTimeout(() => {
          const nextVersion = this.refreshVersion[slideIndex] + 1;
          resolve(createMockItems(slideIndex, nextVersion));
        }, 700);
      });
    },
    collapsePullHeader(slideIndex, time = 600) {
      const ref = this.$refs[`pullHeader-${slideIndex}`];
      const pullHeader = Array.isArray(ref) ? ref[0] : ref;
      if (pullHeader && pullHeader.collapsePullHeader) {
        pullHeader.collapsePullHeader({ time });
      }
    },
    scrollToNextPage() {
      if (this.currentSlide < this.$maxSlideIndex) {
        this.currentSlide = this.currentSlideNum + 1;
      } else {
        this.currentSlide = 0;
      }
    },
    scrollToPrevPage() {
      if (this.currentSlide === 0) {
        this.currentSlide = this.$maxSlideIndex;
      } else {
        this.currentSlide = this.currentSlideNum - 1;
      }
    },
    onDropped(evt) {
      this.currentSlideNum = evt.currentSlide;
      this.currentSlide = evt.currentSlide;
    },
    onHeaderPulling(evt, slideIndex) {
      if (this.refreshingFlags[slideIndex]) {
        return;
      }
      if (evt.contentOffset > 30) {
        this.$set(this.headerRefreshText, slideIndex, 'Release to refresh');
      } else {
        this.$set(this.headerRefreshText, slideIndex, 'Pull down to refresh');
      }
    },
    async onHeaderReleased(slideIndex) {
      if (this.refreshingFlags[slideIndex]) {
        return;
      }

      this.$set(this.refreshingFlags, slideIndex, true);
      this.$set(this.headerRefreshText, slideIndex, 'Refreshing...');

      const data = await this.mockFetchData(slideIndex);
      this.$set(this.listDataSource, slideIndex, data);
      this.$set(this.refreshVersion, slideIndex, this.refreshVersion[slideIndex] + 1);

      this.$set(this.refreshingFlags, slideIndex, false);
      this.$set(this.headerRefreshText, slideIndex, 'Refresh finished');
      this.collapsePullHeader(slideIndex, 700);
    },
  },
};
</script>

<style>
#demo-swiper-list {
  flex: 1;
}

#demo-swiper-list #swiper {
  flex: 1;
}

#demo-swiper-list .swiper-slide {
  flex: 1;
  background-color: #f5f7fa;
}

#demo-swiper-list .slide-list {
  flex: 1;
  background-color: #ffffff;
}

#demo-swiper-list .ul-refresh {
  background-color: #40b883;
}

#demo-swiper-list .ul-refresh-text {
  color: #ffffff;
  height: 50px;
  line-height: 50px;
  text-align: center;
}

#demo-swiper-list .item-style {
  padding: 12px;
  border-bottom-width: 1px;
  border-bottom-color: #e5e5e5;
  border-style: solid;
  background-color: #ffffff;
}

#demo-swiper-list .item-title {
  font-size: 16px;
  color: #222222;
}

#demo-swiper-list .item-subtitle {
  font-size: 12px;
  color: #666666;
  margin-top: 4px;
}

#demo-swiper-list #swiper-dots {
  flex-direction: row;
  align-items: center;
  justify-content: center;
  height: 40px;
}

#demo-swiper-list .dot {
  width: 10px;
  height: 10px;
  border-radius: 5px;
  background-color: darkgray;
  margin-left: 5px;
  margin-right: 5px;
}

#demo-swiper-list .dot.hightlight {
  background-color: limegreen;
}
</style>