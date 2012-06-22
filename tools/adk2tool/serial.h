/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef __SERIAL_H
#define __SERIAL_H

int serial_open(const char *port);
void serial_close(void);
int serial_get_fd(void);
int serial_get_dtr(void);
int serial_get_rts(void);
void serial_set_dtr(int val);
void serial_set_rts(int val);

#endif
