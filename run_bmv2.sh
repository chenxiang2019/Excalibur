#!/bin/bash

# Copyright 2013-present Barefoot Networks, Inc. 
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
#    http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

THIS_DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

source $THIS_DIR/env.sh
echo $BMV2_PATH
#SWITCH_PATH=$BMV2_PATH/targets/simple_switch/simple_switch
SWITCH_PATH=/usr/bin/simple_switch_grpc

CLI_PATH=$THIS_DIR/simple_switch_CLI

p4c --target bmv2 --arch v1model --std p4-16 bmv2/bmv2.p4  -o output
#sudo PYTHONPATH=$PYTHONPATH:$BMV2_PATH/mininet/ 
sudo python3 script/topo.py \
    --behavioral-exe $SWITCH_PATH \
    --json output/bmv2.json \
    --mode l3 
    

echo 11111111
