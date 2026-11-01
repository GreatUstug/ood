//
// Created by maxim on 01.11.2026.
//

#ifndef L4_DOCUMENTREGISTRY_H
#define L4_DOCUMENTREGISTRY_H

#include "Document.h"
#include <memory>
#include <vector>

class DocumentRegistry
{
public:
	std::shared_ptr<Document> Create()
	{
		auto d = std::make_shared<Document>();
		m_docs.push_back(d);
		return d;
	}

	std::shared_ptr<Document> CreateFrom(shapes::Picture&& p, std::string name)
	{
		auto d = std::make_shared<Document>(std::move(p));
		d->SetName(std::move(name));
		m_docs.push_back(d);
		return d;
	}

	void Prune()
	{
		std::erase_if(m_docs, [](const auto& d) { return d.use_count() == 1; });
	}

private:
	std::vector<std::shared_ptr<Document> > m_docs{};
};

#endif //L4_DOCUMENTREGISTRY_H