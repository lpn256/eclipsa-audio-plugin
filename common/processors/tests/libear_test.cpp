// Copyright 2025 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>

#include <iomanip>
#include <iostream>

#include "ear/ear.hpp"

using namespace ear;

TEST(libear, sanity) {
  // make the gain calculator
  Layout layout = getLayout("0+5+0");
  GainCalculatorObjects gc(layout);

  // make the input data; just left of centre
  ObjectsTypeMetadata otm;
  otm.position = PolarPosition(10.0f, 0.0f, 1.0f);

  // calculate the direct and diffuse gains
  std::vector<float> directGains(layout.channels().size());
  std::vector<float> diffuseGains(layout.channels().size());
  gc.calculate(otm, directGains, diffuseGains);

  // print the output
  auto fmt = std::setw(10);
  std::cout << std::setprecision(4);

  std::cout << fmt << "channel"  //
            << fmt << "direct"   //
            << fmt << "diffuse" << std::endl;
  for (size_t i = 0; i < layout.channels().size(); i++)
    std::cout << fmt << layout.channels()[i].name()  //
              << fmt << directGains[i]               //
              << fmt << diffuseGains[i] << std::endl;
}