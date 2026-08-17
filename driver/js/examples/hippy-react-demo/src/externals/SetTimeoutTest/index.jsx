import React from 'react';
import {
  View,
  Text,
  ScrollView,
  StyleSheet,
  Platform,
} from '@hippy/react';

/**
 * SetTimeout nested-delay diagnose page.
 *
 * Demo 1: run `runCount` rounds of `setTimeout -> setTimeout` (both with the
 *         same delay), and record each outer/inner actual elapsed time. Used
 *         to reproduce the reported issue that on OHOS the inner timer can
 *         be delayed by 300~400ms.
 *
 * Demo 2: inside a setTimeout callback, schedule a Promise microtask and
 *         another setTimeout at the same time, then compare their delays.
 *         If the microtask is fast but setTimeout is slow, the problem is
 *         in the timer path (CVDriver / delayed queue), not in JS thread
 *         scheduling.
 */

const styles = StyleSheet.create({
  container: {
    flex: 1,
    padding: 12,
    backgroundColor: '#ffffff',
  },
  header: {
    fontSize: 16,
    color: '#242424',
    marginTop: 8,
    marginBottom: 4,
    fontWeight: 'bold',
  },
  desc: {
    fontSize: 13,
    color: '#666666',
    lineHeight: 20,
    marginBottom: 8,
  },
  paramRow: {
    flexDirection: 'row',
    alignItems: 'center',
    marginBottom: 8,
  },
  paramLabel: {
    fontSize: 13,
    color: '#242424',
    width: 90,
  },
  paramValue: {
    fontSize: 13,
    color: '#4c9afa',
    marginRight: 12,
  },
  btnRow: {
    flexDirection: 'row',
    flexWrap: 'wrap',
    marginBottom: 8,
  },
  btn: {
    paddingLeft: 12,
    paddingRight: 12,
    paddingTop: 6,
    paddingBottom: 6,
    marginRight: 8,
    marginBottom: 8,
    borderWidth: 1,
    borderStyle: 'solid',
    borderColor: '#4c9afa',
    borderRadius: 4,
  },
  btnText: {
    fontSize: 13,
    color: '#4c9afa',
  },
  summary: {
    fontSize: 13,
    color: '#242424',
    marginTop: 4,
    marginBottom: 4,
  },
  summaryBad: {
    color: '#e64340',
  },
  summaryGood: {
    color: '#3cc51f',
  },
  logBox: {
    borderColor: '#dddddd',
    borderWidth: 1,
    borderStyle: 'solid',
    padding: 6,
    marginBottom: 12,
  },
  logLine: {
    fontSize: 12,
    color: '#333333',
    lineHeight: 18,
  },
  logLineBad: {
    fontSize: 12,
    color: '#e64340',
    lineHeight: 18,
  },
});

// Threshold: if a timer overrun exceeds targetDelay + THRESHOLD_MS,
// we treat it as an abnormal delay.
const THRESHOLD_MS = 100;

export default class SetTimeoutTest extends React.Component {
  constructor(props) {
    super(props);
    this.state = {
      runCount: 20,
      targetDelay: 30,
      demo1Logs: [],
      demo1Summary: '',
      demo1Bad: 0,
      demo2Logs: [],
      demo2Summary: '',
      running: false,
    };
    this.runDemo1 = this.runDemo1.bind(this);
    this.runDemo2 = this.runDemo2.bind(this);
    this.clearLogs = this.clearLogs.bind(this);
    this.setDelay = this.setDelay.bind(this);
    this.setRunCount = this.setRunCount.bind(this);
  }

  setDelay(delay) {
    if (this.state.running) return;
    this.setState({ targetDelay: delay });
  }

  setRunCount(count) {
    if (this.state.running) return;
    this.setState({ runCount: count });
  }

  clearLogs() {
    this.setState({
      demo1Logs: [],
      demo1Summary: '',
      demo1Bad: 0,
      demo2Logs: [],
      demo2Summary: '',
    });
  }

  /**
   * Demo 1: nested setTimeout latency reproduction.
   */
  runDemo1() {
    if (this.state.running) return;
    const { runCount, targetDelay } = this.state;
    const logs = [];
    const results = [];
    let done = 0;
    let badCount = 0;

    this.setState({ running: true, demo1Logs: [], demo1Summary: '', demo1Bad: 0 });

    const finish = () => {
      const outerList = results.map(r => r.outer);
      const innerList = results.map(r => r.inner);
      const avg = arr => (arr.reduce((a, b) => a + b, 0) / arr.length).toFixed(1);
      const max = arr => Math.max.apply(null, arr);
      const summary = `total=${runCount}, target=${targetDelay}ms | `
        + `outer avg=${avg(outerList)}ms max=${max(outerList)}ms | `
        + `inner avg=${avg(innerList)}ms max=${max(innerList)}ms | `
        + `bad(inner>${targetDelay + THRESHOLD_MS}ms)=${badCount}`;
      this.setState({
        demo1Logs: logs,
        demo1Summary: summary,
        demo1Bad: badCount,
        running: false,
      });
    };

    for (let i = 0; i < runCount; i++) {
      const tag = i;
      const t0 = Date.now();
      setTimeout(() => {
        const t1 = Date.now();
        const outerDiff = t1 - t0;
        setTimeout(() => {
          const t2 = Date.now();
          const innerDiff = t2 - t1;
          const bad = innerDiff > targetDelay + THRESHOLD_MS;
          if (bad) badCount++;
          results.push({ outer: outerDiff, inner: innerDiff });
          logs.push({
            text: `[case-${tag}] outer=${outerDiff}ms  inner=${innerDiff}ms`
              + (bad ? '  <-- ABNORMAL' : ''),
            bad,
          });
          done++;
          if (done === runCount) finish();
        }, targetDelay);
      }, targetDelay);
    }
  }

  /**
   * Demo 2: microtask vs setTimeout comparison inside the same outer tick.
   */
  runDemo2() {
    if (this.state.running) return;
    const { runCount, targetDelay } = this.state;
    const logs = [];
    let microSum = 0;
    let innerSum = 0;
    let microMax = 0;
    let innerMax = 0;
    let done = 0;

    this.setState({ running: true, demo2Logs: [], demo2Summary: '' });

    const finish = () => {
      const microAvg = (microSum / runCount).toFixed(1);
      const innerAvg = (innerSum / runCount).toFixed(1);
      let hint = '';
      if (innerMax > targetDelay + THRESHOLD_MS && microMax < THRESHOLD_MS) {
        hint = ' => timer path is slow, JS thread is OK (likely CVDriver / delayed queue issue)';
      } else if (microMax > THRESHOLD_MS) {
        hint = ' => JS thread itself is blocked / low priority';
      } else {
        hint = ' => no reproduction this round';
      }
      this.setState({
        demo2Logs: logs,
        demo2Summary: `microtask avg=${microAvg}ms max=${microMax}ms | `
          + `nested setTimeout avg=${innerAvg}ms max=${innerMax}ms${hint}`,
        running: false,
      });
    };

    for (let i = 0; i < runCount; i++) {
      const tag = i;
      setTimeout(() => {
        const t1 = Date.now();
        let microDiff = -1;
        let innerDiff = -1;
        let sub = 0;

        const tryFinish = () => {
          if (sub === 2) {
            const bad = innerDiff > targetDelay + THRESHOLD_MS;
            logs.push({
              text: `[case-${tag}] microtask=${microDiff}ms  nested setTimeout=${innerDiff}ms`
                + (bad ? '  <-- ABNORMAL' : ''),
              bad,
            });
            microSum += microDiff;
            innerSum += innerDiff;
            if (microDiff > microMax) microMax = microDiff;
            if (innerDiff > innerMax) innerMax = innerDiff;
            done++;
            if (done === runCount) finish();
          }
        };

        Promise.resolve().then(() => {
          microDiff = Date.now() - t1;
          sub++;
          tryFinish();
        });

        setTimeout(() => {
          innerDiff = Date.now() - t1;
          sub++;
          tryFinish();
        }, targetDelay);
      }, targetDelay);
    }
  }

  renderLog(logs, prefix) {
    if (!logs.length) return null;
    return logs.map((item, idx) => (
      <Text
        key={`${prefix}-${idx}`}
        style={item.bad ? styles.logLineBad : styles.logLine}
      >
        {item.text}
      </Text>
    ));
  }

  render() {
    const {
      runCount,
      targetDelay,
      demo1Logs,
      demo1Summary,
      demo1Bad,
      demo2Logs,
      demo2Summary,
      running,
    } = this.state;

    const delayOptions = [10, 30, 50, 100];
    const countOptions = [10, 20, 50];

    return (
      <ScrollView style={styles.container}>
        <Text style={styles.header}>Platform: {Platform.OS}</Text>

        <Text style={styles.header}>Parameters</Text>
        <View style={styles.paramRow}>
          <Text style={styles.paramLabel}>Target delay:</Text>
          {delayOptions.map(d => (
            <View
              key={`delay-${d}`}
              style={styles.btn}
              onClick={() => this.setDelay(d)}
            >
              <Text
                style={[
                  styles.btnText,
                  targetDelay === d ? { fontWeight: 'bold' } : null,
                ]}
              >
                {d}ms
              </Text>
            </View>
          ))}
        </View>
        <View style={styles.paramRow}>
          <Text style={styles.paramLabel}>Run count:</Text>
          {countOptions.map(c => (
            <View
              key={`count-${c}`}
              style={styles.btn}
              onClick={() => this.setRunCount(c)}
            >
              <Text
                style={[
                  styles.btnText,
                  runCount === c ? { fontWeight: 'bold' } : null,
                ]}
              >
                {c}
              </Text>
            </View>
          ))}
        </View>

        <View style={styles.btnRow}>
          <View style={styles.btn} onClick={this.runDemo1}>
            <Text style={styles.btnText}>Run Demo 1 (nested setTimeout)</Text>
          </View>
          <View style={styles.btn} onClick={this.runDemo2}>
            <Text style={styles.btnText}>Run Demo 2 (microtask vs setTimeout)</Text>
          </View>
          <View style={styles.btn} onClick={this.clearLogs}>
            <Text style={styles.btnText}>Clear</Text>
          </View>
        </View>

        {running ? <Text style={styles.desc}>Running, please wait...</Text> : null}

        <Text style={styles.header}>Demo 1: nested setTimeout latency</Text>
        <Text style={styles.desc}>
          Fires `runCount` outer setTimeout(targetDelay). Each outer callback
          fires an inner setTimeout(targetDelay). Records outer/inner actual
          elapsed time. Expected value is close to targetDelay; large inner
          delays indicate the bug.
        </Text>
        {demo1Summary ? (
          <Text
            style={[
              styles.summary,
              demo1Bad > 0 ? styles.summaryBad : styles.summaryGood,
            ]}
          >
            {demo1Summary}
          </Text>
        ) : null}
        <View style={styles.logBox}>
          {this.renderLog(demo1Logs, 'd1')}
        </View>

        <Text style={styles.header}>Demo 2: microtask vs setTimeout</Text>
        <Text style={styles.desc}>
          Inside each outer setTimeout callback, schedules a Promise
          microtask and a nested setTimeout at the same time. If microtask
          stays fast while nested setTimeout gets slow, the JS thread is
          fine and the problem is in the timer path (CVDriver / delayed
          task queue).
        </Text>
        {demo2Summary ? <Text style={styles.summary}>{demo2Summary}</Text> : null}
        <View style={styles.logBox}>
          {this.renderLog(demo2Logs, 'd2')}
        </View>
      </ScrollView>
    );
  }
}
