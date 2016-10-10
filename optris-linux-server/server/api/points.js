import moment from 'moment';

JsonRoutes.add('get', '/api/points', (req, res) => {
  Points.insert({
    type: req.query.type,
    value: parseFloat(req.query.value),
    date: moment(req.query.date, 'DD/MM/YYYY HH:mm:ss').toDate()
  });

  JsonRoutes.sendResult(res, {
    success: true
  });
});
