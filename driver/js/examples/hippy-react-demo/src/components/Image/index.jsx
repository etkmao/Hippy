import React from 'react';
import {
  Text,
  View,
  ScrollView,
  StyleSheet,
  Platform,
} from '@hippy/react';

const stylesNoOffset = StyleSheet.create({
  shadowDemo: {
    flex: 1,
    overflowY: 'scroll',
  },
  // android boxShadow style
  shadowDemoCubeAndroid: {
    position: 'absolute',
    left: 50,
    top: 50,
    width: 170,
    height: 270,
    boxShadowOpacity: 0.6,
//     boxShadowRadius: 5,
    boxShadowColor: '#4c9afa',
    // container & content should both set radius
    // 容器和内容都要设置radius
    borderRadius: 5,
    backgroundColor: '#0055ff',
    paddingLeft: 15,
    paddingRight: 15,
    paddingTop: 20,
    paddingBottom: 20,
  },

  text: {
    color: 'white',
  },
});

const styles = StyleSheet.create({
  itemStyle: {
    width: 100,
    height: 100,
    lineHeight: 100,
    borderWidth: 1,
    borderStyle: 'solid',
    borderColor: '#4c9afa',
    fontSize: 80,
    margin: 20,
    color: '#4c9afa',
    textAlign: 'center',
  },
  verticalScrollView: {
//     height: 300,
    width: 140,
//     margin: 20,
    borderColor: '#eee',
    borderWidth: 1,
    borderStyle: 'solid',
    backgroundColor: '#aa5500'
  },
});

export default function BoxShadowExpo() {
  return (
    <View style={stylesNoOffset.shadowDemo}>

        <View style={stylesNoOffset.shadowDemoCubeAndroid}>

          <ScrollView
            bounces={true}
            horizontal={false}
            style={styles.verticalScrollView}
          >
            <Text style={styles.itemStyle}>A</Text>
            <Text style={styles.itemStyle}>B</Text>
            <Text style={styles.itemStyle}>C</Text>
            <Text style={styles.itemStyle}>D</Text>
            <Text style={styles.itemStyle}>E</Text>
            <Text style={styles.itemStyle}>F</Text>
            <Text style={styles.itemStyle}>A</Text>
          </ScrollView>

        </View>

    </View>
  );
}
