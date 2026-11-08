//
// Created by maxim on 31.10.2026.
//

#ifndef L4_DOCUMENT_H
#define L4_DOCUMENT_H
#include "Picture.h"
#include "Commands/CommandHistory.h"

class Document
{
public:
	Document() = default;

	explicit Document(shapes::Picture&& p)
		: m_picture(std::move(p))
	{
	}

	shapes::Picture& GetPicture() { return m_picture; }
	const shapes::Picture& GetPicture() const { return m_picture; }
	CommandHistory& GetHistory() { return m_history; }

	const std::string& GetName() const { return m_name; }
	void SetName(std::string n) { m_name = std::move(n); }

private:
	shapes::Picture m_picture;
	CommandHistory m_history;
	std::string m_name = "Unsaved";
};
#endif //L4_DOCUMENT_H