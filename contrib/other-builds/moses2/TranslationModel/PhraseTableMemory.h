/*
 * PhraseTableMemory.h
 *
 *  Created on: 28 Oct 2015
 *      Author: hieu
 */

#ifndef PHRASETABLEMEMORY_H_
#define PHRASETABLEMEMORY_H_
#include "PhraseTable.h"
#include "../legacy/Util2.h"

namespace Moses2
{

class PhraseTableMemory : public PhraseTable
{
//////////////////////////////////////
	class Node
	{
	public:
		Node();
		~Node();
		void AddRule(PhraseImpl &source, TargetPhrase *target);
		TargetPhrases *Find(const Phrase &source, size_t pos = 0) const;

		void SortAndPrune(size_t tableLimit, MemPool &pool, System &system);

	protected:
		typedef boost::unordered_map<Word, Node, UnorderedComparer<Word>, UnorderedComparer<Word> > Children;
		Children m_children;
		TargetPhrases *m_targetPhrases;
		PhraseImpl *m_source;
		std::vector<TargetPhrase*> *m_unsortedTPS;

		Node &AddRule(PhraseImpl &source, TargetPhrase *target, size_t pos);

	};
//////////////////////////////////////
public:
	PhraseTableMemory(size_t startInd, const std::string &line);
	virtual ~PhraseTableMemory();

	virtual void Load(System &system);
	virtual TargetPhrases *Lookup(const Manager &mgr, MemPool &pool, InputPath &inputPath) const;

protected:
	Node m_root;
};

}


#endif /* PHRASETABLEMEMORY_H_ */
