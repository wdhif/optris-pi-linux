import Chart from 'chart.js';

const types = ['min', 'avg', 'max'];

Template.index.onCreated(function () {
  this.points = {};
  _.each(types, type => this.points[type] = new ReactiveVar([]));

  this.autorun(() => {
    this.subscribe('chart');
    _.each(types, type => this.points[type].set(Points.find({ type }, { sort: { date: 1 } }).fetch()));
  });
});

Template.index.helpers({
  last: type => {
    const results = Points.find({ type }, { sort: { date: -1 }, limit: 1 }).fetch();
    return results.length === 1 ? results[0].value : null;
  }
});

Template.index.onRendered(function () {
  const ctx = document.getElementById('chart').getContext('2d');
  let chart = null;

  this.autorun(() => {
    const self = this;

    if (chart) {
      chart.destroy();
    }

    chart = new Chart(ctx, {
      type: 'line',
      data: {
        datasets: [{
          label: 'Minimum temperature',
          fill: false,
          borderColor: 'blue',
          backgroundColor: 'blue',
          tension: 0,
          data: _.map(self.points.min.get(), point => ({
            x: point.date,
            y: point.value
          }))
        }, {
          label: 'Average temperature',
          fill: false,
          borderColor: 'black',
          backgroundColor: 'black',
          tension: 0,
          data: _.map(self.points.avg.get(), point => ({
            x: point.date,
            y: point.value
          }))
        }, {
          label: 'Maximum temperature',
          fill: false,
          borderColor: 'red',
          backgroundColor: 'red',
          tension: 0,
          data: _.map(self.points.max.get(), point => ({
            x: point.date,
            y: point.value
          }))
        }]
      },
      options: {
        animation: false,
        scales: {
          xAxes: [{
            type: 'time',
            position: 'bottom'
          }]
        }
      }
    });
  });
});
