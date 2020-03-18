#include "match.h"

cv::Mat compute_distance(cv::Mat feat1, cv::Mat feat2) {
  int n = feat1.rows;
  int m = feat2.rows;
  cv::Mat dist_norm = cv::Mat::zeros(n, m, CV_64F);

  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      double t = 0;
      for (int k = 0; k < 128; k++) {
        t += (feat1.at<double>(i, k) - feat2.at<double>(j, k)) * (feat1.at<double>(i, k) - feat2.at<double>(j, k));
      }
      dist_norm.at<double>(i, j) = sqrt(t);
    }
  }

  return dist_norm;
}

template<typename T>

std::vector<int> argsort(const std::vector<T>& a){
  int Len = a.size();
  std::vector<int> idx(Len, 0);
  for(int i = 0; i < Len; i++){
    idx[i] = i;
  }
  std::sort(idx.begin(), idx.end(), [&a](int i1, int i2){return a[i1] < a[i2];});
  return idx;
}

std::vector<cv::DMatch> match_features(cv::Mat& img1, cv::Mat& img2,
                                       cv::Mat& feat1, cv::Mat& feat2,
                                       std::vector<cv::KeyPoint>& kp1, std::vector<cv::KeyPoint>& kp2) {
  cv::Ptr<cv::ORB> orb = cv::ORB::create();
  orb->detect(img1, kp1);
  orb->detect(img2, kp2);
  orb->compute(img1, kp1, feat1);
  orb->compute(img2, kp2, feat2);

  std::vector<cv::DMatch> matches;
  cv::BFMatcher matcher(cv::NORM_HAMMING);
  matcher.match(feat1, feat2, matches);

  // ssd distance
  cv::Mat dists = compute_distance(feat1, feat2);
  int n = feat1.rows;
  int m = feat2.rows;
  int nn_sort[n][2];
  double nn1[n], nn2[n], ratio[n];
  for (int i = 0; i < n; i++) {
    nn1[i] = DBL_MAX, nn2[i] = DBL_MAX;
    nn_sort[i][0] = -1, nn_sort[i][1] = -1;
    for (int j = 0; j < m; j++) {
      double t = dists.at<double>(i, j);
      if (t < nn1[i] && nn2[i] == DBL_MAX) {
        nn1[i] = t;
        nn_sort[i][0] = j;
      } else if (t < nn1[i]) {
        nn2[i] = nn1[i], nn1[i] = t;
        nn_sort[i][1] = nn_sort[i][0], nn_sort[i][0] = j;
      } else if (t < nn2[i]) {
        nn2[i] = t;
        nn_sort[i][1] = j;
      }
    }
    ratio[i] = nn1[i] / nn2[i];
  }

  // ratio test
  std::vector<int> r_vec;
  for (int i = 0; i < n; i++) r_vec.emplace_back(ratio[i]);
  std::vector<int> r_sort = argsort(r_vec);

  int cnt = 0;
  for (int i = 0; i < n; i++) {
    if (ratio[i] <= 0.9) {
      cnt++;
    }
  }

  cv::Mat rank = cv::Mat::zeros(cnt, 2, CV_32S);
  for (int i = 0; i < cnt; i++) {
    rank.at<int>(i, 0) = r_sort[i];
    rank.at<int>(i, 1) = nn_sort[r_sort[i]][0];
  }

  return matches;
}