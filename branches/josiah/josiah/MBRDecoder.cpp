#include "MBRDecoder.h"

#include <sstream>
#include <map>
#include "SentenceBleu.h"

using namespace std;

namespace Josiah {

  
  

 pair<const Translation*,float> MBRDecoder::getMax() {
  map<const Translation*, double> p;
  getDistribution(p);
  
  //sort the translations by their probability
  map<const Translation*, double>::const_iterator ci;
  multimap<float, const Translation*,greater<float> > sorted;
  
  for (ci = p.begin(); ci != p.end(); ++ci) {
    sorted.insert(make_pair<float, const Translation*>(ci->second, ci->first));
  }
  
  multimap<float, const Translation*,greater<float> >::iterator i;
  for (i = sorted.begin(); i != sorted.end(); ++i)
    VERBOSE(2, i->first << "\t" << ToString(*i->second) << endl);
  
  //Posterior probs computed using the whole evidence set
  //MBR decoding outer loop using configurable size
  vector<pair<const Translation*, float> > topNTranslations;
  
  for (i = sorted.begin(); i != sorted.end(); ++i) {
    topNTranslations.push_back(make_pair(i->second, i->first));
    g.push_back(new SentenceBLEU(4,*i->second)); //Calc the sufficient statistics for the translation
  }
  
  vector<pair<const Translation*, float> >::iterator it;
  for (it = topNTranslations.begin(); it != topNTranslations.end(); ++it)
    VERBOSE(1,  ToString(*it->first) << endl);
  
  //Main MBR computation done here
  float bleu(0.0), weightedLoss(0.0), weightedLossCumul(0.0), minMBRLoss(100000);
  vector<float> mbrLoss;
  int minMBRLossIdx(-1);
  mbrSize = min(mbrSize, (int) topNTranslations.size());
  cerr << "MBR SIZE " << mbrSize << ", all Translations Size " << topNTranslations.size() << endl;
  
  //Outer loop using only the top #mbrSize samples 
  for(int i = 0; i < mbrSize; ++i) {
    weightedLossCumul = 0.0;
    const GainFunction& gf = g[i];
    VERBOSE(2, "Reference " << ToString(*topNTranslations[i].first) << endl);
    for(size_t j = 0; j < topNTranslations.size(); ++j) {//Inner loop using all samples
      if (static_cast<size_t>(i) != j) {
        bleu = gf.ComputeGain(g[j]);
        VERBOSE(2, "Hypothesis " << ToString(*topNTranslations[j].first) << endl);
        weightedLoss = (1- bleu) * topNTranslations[j].second;
        VERBOSE(2, "Bleu " << bleu << ", prob " <<  topNTranslations[j].second << ", weightedLoss : " << weightedLoss << endl);
        weightedLossCumul += weightedLoss;
        if (weightedLossCumul > minMBRLoss)
          break;
      }
    }
    VERBOSE(2, "Bayes risk for cand " << i << " " <<  weightedLossCumul << endl);
    if (weightedLossCumul < minMBRLoss){
      minMBRLoss = weightedLossCumul;
      minMBRLossIdx = i;
    }
  }
  VERBOSE(2, "Minimum Bayes risk cand is " <<  minMBRLossIdx << " with risk " << minMBRLoss << endl);
  
  return topNTranslations[minMBRLossIdx];
}

}
