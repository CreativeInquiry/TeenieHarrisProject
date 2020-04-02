    cv::Mat border; im.copyTo(border);
    int cnt = im.rows * im.cols - (im.rows-d*2)*(im.cols-d*2);
    cv::Rect inner(d,d,im.cols-d*2,im.rows-d*2);
    border(inner) = cv::Scalar(0);
    float avg = (cv::sum(border)/(float)cnt)[0];
    if (mod){
      border = cv::Scalar(avg);
      im(inner).copyTo(border(inner));
      border.copyTo(im);
    }
    return avg;