/**
 *
 * \section COPYRIGHT
 *
 * Copyright 2013-2015 Software Radio Systems Limited
 *
 * \section LICENSE
 *
 * This file is part of the srsUE library.
 *
 * srsUE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * srsUE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * A copy of the GNU Affero General Public License can be found in
 * the LICENSE file in the top-level directory of this distribution
 * and at http://www.gnu.org/licenses/.
 *
 */

#define NTHREADS 100
#define NMSGS    100

#include <stdio.h>
#include "common/logger.h"

using namespace srsue;

typedef struct {
  logger *l;
  int thread_id;
}args_t;

void* thread_loop(void *a) {
  args_t *args = (args_t*)a;
  char buf[100];
  for(int i=0;i<NMSGS;i++)
  {
    sprintf(buf, "Thread %d: %d", args->thread_id, i);
    args->l->log(buf);
  }
}

void write(std::string filename) {
  logger l;
  l.init(filename);
  pthread_t threads[NTHREADS];
  args_t    args[NTHREADS];
  for(int i=0;i<NTHREADS;i++) {
    args[i].l = &l;
    args[i].thread_id = i;
    pthread_create(&threads[i], NULL, &thread_loop, &args[i]);
  }
  for(int i=0;i<NTHREADS;i++) {
    pthread_join(threads[i], NULL);
  }
}

bool read(std::string filename) {
  bool pass = true;
  bool written[NTHREADS][NMSGS];
  int thread, msg;
  int r;

  for(int i=0;i<NTHREADS;i++) {
    for(int j=0;j<NMSGS;j++) {
      written[i][j] = false;
    }
  }
  FILE *f = fopen(filename.c_str(), "r");
  if(f!=NULL) {
    while(fscanf(f, "Thread %d: %d\n", &thread, &msg)) {
      written[thread][msg] = true;
    }
    fclose(f);
  }
  for(int i=0;i<NTHREADS;i++) {
    for(int j=0;j<NMSGS;j++) {
      if(!written[i][j]) pass = false;
    }
  }
  return pass;
}

int main(int argc, char **argv) {
  bool result;
  std::string f("log.txt");
  write(f);
  result = read(f);
  remove(f.c_str());
  if(result) {
    printf("Passed\n");
    exit(0);
  }else{
    printf("Failed\n;");
    exit(1);
  }
}
