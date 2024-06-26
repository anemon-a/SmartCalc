#include "../model/calculator.h"

s21::Calculator::Calculator() { CreateTokenMap(token_map_); }

s21::Calculator::~Calculator() {}

void s21::Calculator::ParseToTokens(const std::string expression) {
  infix_ = {};
  token_stack_ = {};
  size_t i = 0;
  while (i < expression.length()) {
    if (std::isblank(expression[i])) {
      i++;
      continue;

    } else if (std::isdigit(expression[i])) {
      std::string number = FindNumber(expression, i);
      infix_.push(Token(number, kNumber, None, nullptr));

    } else if (expression[i] == 'x' || expression[i] == 'X') {
      infix_.push(Token("x", kNumber, None, nullptr));
      i++;

    } else if (std::isalpha(expression[i])) {
      std::string temp = FindFunction(expression, i);
      CheckAndPushToken(temp);
    } else {
      if (!FindUnaryMinus(expression, i)) {
        std::string temp{expression[i++]};
        CheckAndPushToken(temp);
      }
    }
  }
}

bool s21::Calculator::FindUnaryMinus(const std::string &exp, size_t &index) {
  if ((exp[index] == '-' || exp[index] == '+') &&
      (index == 0 || exp[index - 1] == '(' || exp[index - 1] == '^')) {
    if (exp[index] == '-') CheckAndPushToken("~");
    index++;
    return true;
  }
  return false;
}

std::string s21::Calculator::FindNumber(const std::string &exp, size_t &index) {
  std::regex r("\\d+(([.]\\d+)?(e([+-])?\\d+)?)?");
  std::sregex_iterator i =
      std::sregex_iterator(exp.begin() + index, exp.end(), r);
  std::smatch match = *i;
  index += match.length();
  return match.str();
}

std::string s21::Calculator::FindFunction(const std::string &exp,
                                          size_t &index) {
  std::regex r("([a-z]+)");
  std::sregex_iterator i =
      std::sregex_iterator(exp.begin() + index, exp.end(), r);
  std::smatch match = *i;
  index += match.length();
  return match.str();
}

void s21::Calculator::CheckAndPushToken(std::string temp) {
  if (auto it = token_map_.find(temp); it != token_map_.end()) {
    infix_.push(it->second);
  } else {
    throw std::logic_error("Incorrect symbol: " + temp);
  }
}

void s21::Calculator::ConvertToPostfix() {
  postfix_ = {};
  while (!infix_.empty()) {
    switch (infix_.front().GetTokenType()) {
      case kNumber:
        postfix_.push(infix_.front());
        if (!token_stack_.empty() && token_stack_.top().GetName() == "~") {
          postfix_.push(token_stack_.top());
          token_stack_.pop();
        }
        break;
      case kOperator:
      case kUnaryOperator:
        while (!token_stack_.empty() && token_stack_.top().GetPriority() >=
                                            infix_.front().GetPriority()) {
          postfix_.push(token_stack_.top());
          token_stack_.pop();
        }
        token_stack_.push(infix_.front());
        break;
      case kFunction:
      case kOpenBracket:
        token_stack_.push(infix_.front());
        break;
      case kCloseBracket:
        while (!token_stack_.empty() &&
               token_stack_.top().GetTokenType() != kOpenBracket) {
          postfix_.push(token_stack_.top());
          token_stack_.pop();
        }
        if (!token_stack_.empty() && token_stack_.top().GetName() == "(")
          token_stack_.pop();
        else
          throw std::logic_error("Open bracket missing!");
        if (!token_stack_.empty() &&
            token_stack_.top().GetTokenType() == kFunction) {
          postfix_.push(token_stack_.top());
          token_stack_.pop();
        }
        break;
    }
    infix_.pop();
  }
  while (!token_stack_.empty()) {
    if (token_stack_.top().GetName() == "(")
      throw std::logic_error("Excess open bracket!");
    postfix_.push(token_stack_.top());
    token_stack_.pop();
  }
}

double s21::Calculator::CalculatePostfix(double x) {
  double a = 0, b = 0;
  infix_ = postfix_;
  while (!infix_.empty()) {
    switch (infix_.front().GetTokenType()) {
      case kNumber:
        if (infix_.front().GetName() == "x")
          number_stack_.push(x);
        else
          number_stack_.push(std::stod(infix_.front().GetName()));
        break;
      case kOperator:
        b = PopFromResult();
        a = PopFromResult();
        f_ = infix_.front().GetOperator();
        number_stack_.push(f_(a, b));
        break;
      case kFunction:
      case kUnaryOperator:
        a = PopFromResult();
        f_ = infix_.front().GetOperator();
        number_stack_.push(f_(a, 0));
        break;
      default:
        break;
    }
    infix_.pop();
  }
  if (number_stack_.empty()) {
    throw std::logic_error("Incorrect input!");
  } else {
    double result = number_stack_.top();
    number_stack_.pop();
    if (!number_stack_.empty()) throw std::logic_error("Incorrect input!");
    return result;
  }
}

double s21::Calculator::PopFromResult() {
  if (number_stack_.empty()) throw std::logic_error("Incorrect input!");
  double res = number_stack_.top();
  number_stack_.pop();
  return res;
}

void s21::Calculator::CalculateCoordinates(int number_of_points, double x_start,
                                           double x_end, double y_min,
                                           double y_max) {
  std::vector<double> x, y;
  double step = (x_end - x_start) / (number_of_points - 1);
  long double threshold = fabs(step * 1000.0);

  for (int i = 0; i < number_of_points; ++i) {
    x.push_back(x_start + step * i);
    double y_value = CalculatePostfix(x.back());
    long double delta = 0;
    if (i && !!std::isnan(y.back())) {
      delta = fabs((y_value - y.back()) / step);
    }
    if (y_value > y_max || y_value < y_min) {
      y.push_back(std::numeric_limits<double>::quiet_NaN());
    } else if (delta > threshold && (y_value * y.back()) < 0) {
      y.push_back(std::numeric_limits<double>::quiet_NaN());
    } else {
      y.push_back(y_value);
    }
  }
  coordinates_ = std::make_pair(x, y);
}

s21::Calculator::CoordinatesXY &s21::Calculator::GetGraphCoordinates() {
  return coordinates_;
}
