Meteor.publish('chart', () => {
  return Points.find({}, {
    sort: {
      date: -1
    },
    limit: 1000
  });
});
