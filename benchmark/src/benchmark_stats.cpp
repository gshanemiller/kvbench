#include <benchmark_stats.h>

#include <assert.h>

void Benchmark::Stats::addResultSet(
    const char *description,                                                                            
    unsigned long iterations,                                                                           
    timespec start,                                          
    timespec end,                                            
    u_int64_t pmuFixedCounter0Value,                                                                    
    u_int64_t pmuFixedCounter1Value,                                                                    
    u_int64_t pmuFixedCounter2Value,                                                                    
    u_int64_t pmuProgrammableCounter0Value,                                                             
    u_int64_t pmuProgrammableCounter1Value,                                                             
    u_int64_t pmuProgrammableCounter2Value,                                                             
    u_int64_t pmuProgrammableCounter3Value)
{
  d_description.push_back(description);
  d_itertions.push_back(iterations);
  d_fixedCntr0.push_back(pmuFixedCounter0Value); 
  d_fixedCntr1.push_back(pmuFixedCounter1Value); 
  d_fixedCntr2.push_back(pmuFixedCounter2Value); 
  d_progmCntr0.push_back(pmuProgrammableCounter0Value);
  d_progmCntr1.push_back(pmuProgrammableCounter1Value);
  d_progmCntr2.push_back(pmuProgrammableCounter2Value);
  d_progmCntr3.push_back(pmuProgrammableCounter3Value);

  // Calc elapased time
  assert(end.tv_sec>=start.tv_sec);
  unsigned long nsDiff(0);
  unsigned long secDiff = end.tv_sec - start.tv_sec;
  if (end.tv_nsec < start.tv_nsec) {
    --secDiff; // borrow a second
    nsDiff = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    nsDiff = end.tv_nsec - start.tv_nsec;
  }
  d_elapsedNs.push_back(static_cast<double>(secDiff*1000000000+nsDiff));

  assert(d_description.size()==d_itertions.size());
  assert(d_description.size()==d_fixedCntr0.size());
  assert(d_description.size()==d_fixedCntr1.size());
  assert(d_description.size()==d_fixedCntr2.size());
  assert(d_description.size()==d_progmCntr0.size());
  assert(d_description.size()==d_progmCntr1.size());
  assert(d_description.size()==d_progmCntr2.size());
  assert(d_description.size()==d_progmCntr3.size());
  assert(d_description.size()==d_elapsedNs.size());
}

void Benchmark::Stats::summarize(const Intel::SkyLake::PMU& pmu, unsigned i) const {
  double perIterF0 = static_cast<double>(d_fixedCntr0[i])/static_cast<double>(d_itertions[i]);
  double perIterF1 = static_cast<double>(d_fixedCntr1[i])/static_cast<double>(d_itertions[i]);
  double perIterF2 = static_cast<double>(d_fixedCntr2[i])/static_cast<double>(d_itertions[i]);
  double perIterP0 = static_cast<double>(d_progmCntr0[i])/static_cast<double>(d_itertions[i]);
  double perIterP1 = static_cast<double>(d_progmCntr1[i])/static_cast<double>(d_itertions[i]);
  double perIterP2 = static_cast<double>(d_progmCntr2[i])/static_cast<double>(d_itertions[i]);
  double perIterP3 = static_cast<double>(d_progmCntr3[i])/static_cast<double>(d_itertions[i]);
  double bnchRatio = perIterP2/perIterF0;
  double bnchWaste = perIterP3/perIterF0;
  double perIter   = d_elapsedNs[i]/static_cast<double>(d_itertions[i]);
  double opsSec    = static_cast<double>(1000000000)/perIter;

  printf("    {\n");
  printf("      \"description\": \"%s\",\n", d_description[i].c_str()); 
  printf("      \"total\": {\n");
  printf("        \"iterations\": %lu,\n", d_itertions[i]);
  printf("        \"elapsedTimeNs\": %lf,\n", d_elapsedNs[i]); 
  printf("        \"%s\": %lu,\n", pmu.fixedMnemonic()[0].c_str(), d_fixedCntr0[i]);
  printf("        \"%s\": %lu,\n", pmu.fixedMnemonic()[1].c_str(), d_fixedCntr1[i]);
  printf("        \"%s\": %lu,\n", pmu.fixedMnemonic()[2].c_str(), d_fixedCntr2[i]);
  printf("        \"%s\": %lu,\n", pmu.progMnemonic()[0].c_str(), d_progmCntr0[i]);
  printf("        \"%s\": %lu,\n", pmu.progMnemonic()[1].c_str(), d_progmCntr1[i]);
  printf("        \"%s\": %lu,\n", pmu.progMnemonic()[2].c_str(), d_progmCntr2[i]);
  printf("        \"%s\": %lu,\n", pmu.progMnemonic()[3].c_str(), d_progmCntr3[i]);
  printf("      },\n");
  printf("      scaledPerIteration: {\n");
  printf("        \"elapsedTimeNs\": %lf,\n", perIter);
  printf("        \"opsPerSecond\" : %lf,\n", opsSec);
  printf("        \"%s\": %lf,\n", pmu.fixedMnemonic()[0].c_str(), perIterF0);
  printf("        \"%s\": %lf,\n", pmu.fixedMnemonic()[1].c_str(), perIterF1);
  printf("        \"%s\": %lf,\n", pmu.fixedMnemonic()[2].c_str(), perIterF2);
  printf("        \"%s\": %lf,\n", pmu.progMnemonic()[0].c_str(), perIterP0);
  printf("        \"%s\": %lf,\n", pmu.progMnemonic()[1].c_str(), perIterP1);
  printf("        \"%s\": %lf,\n", pmu.progMnemonic()[2].c_str(), perIterP2);
  printf("        \"%s\": %lf,\n", pmu.progMnemonic()[3].c_str(), perIterP3);
  printf("        \"IPC\": %lf,\n", perIterF0/perIterF2);
  printf("        \"branchRatio\": %lf\n", bnchRatio);
  printf("        \"branchWasteRatio\": %lf\n", bnchWaste);
  printf("      },\n");
  printf("    }\n");
}

void Benchmark::Stats::legend(const Intel::SkyLake::PMU& pmu) const {
  printf("    \"%s\": \"%s\",\n", pmu.fixedMnemonic()[0].c_str(), pmu.fixedDescription()[0].c_str());
  printf("    \"%s\": \"%s\",\n", pmu.fixedMnemonic()[1].c_str(), pmu.fixedDescription()[1].c_str());
  printf("    \"%s\": \"%s\",\n", pmu.fixedMnemonic()[2].c_str(), pmu.fixedDescription()[2].c_str());
  printf("    \"%s\": \"%s\",\n", pmu.progMnemonic()[0].c_str(), pmu.progDescription()[0].c_str());
  printf("    \"%s\": \"%s\",\n", pmu.progMnemonic()[1].c_str(), pmu.progDescription()[1].c_str());
  printf("    \"%s\": \"%s\",\n", pmu.progMnemonic()[2].c_str(), pmu.progDescription()[2].c_str());
  printf("    \"%s\": \"%s\",\n", pmu.progMnemonic()[3].c_str(), pmu.progDescription()[3].c_str());
  printf("    \"IPC\": \"%s/%s\",\n", pmu.fixedMnemonic()[0].c_str(), pmu.fixedMnemonic()[2].c_str());
  printf("    \"branchRatio\": \"%s/%s\",\n", pmu.progMnemonic()[2].c_str(), pmu.fixedMnemonic()[0].c_str());
  printf("    \"branchWasteRatio\": \"%s/%s\",\n", pmu.progMnemonic()[3].c_str(), pmu.fixedMnemonic()[0].c_str());
}

void Benchmark::Stats::print(const Intel::SkyLake::PMU& pmu) const {
  d_config.print();
  printf("\"stats\": {\n");

  printf("  \"legend\" = {\n");
  legend(pmu);
  printf("  },\n");

  printf("  \"result\" = [\n");
  for (unsigned i=0; i<d_description.size(); ++i) {
    summarize(pmu, i);
  }
  printf("  ]\n");

  printf("}\n");
}
