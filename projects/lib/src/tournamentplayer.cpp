/*
    This file is part of Cute Chess.
    Copyright (C) 2008-2018 Cute Chess authors

    Cute Chess is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Cute Chess is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Cute Chess.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "tournamentplayer.h"
#include "openingbook.h"

#include <QJsonArray>


TournamentPlayer::TournamentPlayer(PlayerBuilder* builder,
				   const TimeControl& timeControl,
				   const OpeningBook* book,
				   int bookDepth)
	: m_builder(builder),
	  m_timeControl(timeControl),
	  m_book(book),
	  m_bookDepth(bookDepth),
	  m_wins(0),
	  m_draws(0),
	  m_losses(0),
	  m_whiteWins(0),
	  m_whiteDraws(0),
	  m_whiteLosses(0),
	  m_terminations(24),
	  m_outcome()
{
	Q_ASSERT(builder != nullptr);
}

TournamentPlayer::TournamentPlayer()
	: m_builder(nullptr),
	  m_timeControl{},
	  m_book(nullptr),
	  m_bookDepth(0),
	  m_wins(0),
	  m_draws(0),
	  m_losses(0),
	  m_whiteWins(0),
	  m_whiteDraws(0),
	  m_whiteLosses(0),
	  m_terminations(24),
	  m_outcome()
{
}

const PlayerBuilder* TournamentPlayer::builder() const
{
	return m_builder;
}

QString TournamentPlayer::name() const
{
	return m_builder->name();
}

void TournamentPlayer::setName(const QString& name)
{
	if (m_builder)
		m_builder->setName(name);
}

const TimeControl& TournamentPlayer::timeControl() const
{
	return m_timeControl;
}

const OpeningBook* TournamentPlayer::book() const
{
	return m_book;
}

int TournamentPlayer::bookDepth() const
{
	return m_bookDepth;
}

int TournamentPlayer::wins() const
{
	return m_wins;
}

int TournamentPlayer::draws() const
{
	return m_draws;
}

int TournamentPlayer::losses() const
{
	return m_losses;
}

int TournamentPlayer::whiteWins() const
{
	return m_whiteWins;
}

int TournamentPlayer::whiteDraws() const
{
	return m_whiteDraws;
}

int TournamentPlayer::whiteLosses() const
{
	return m_whiteLosses;
}

int TournamentPlayer::blackWins() const
{
	return m_wins - m_whiteWins;
}

int TournamentPlayer::blackDraws() const
{
	return m_draws - m_whiteDraws;
}

int TournamentPlayer::blackLosses() const
{
	return m_losses - m_whiteLosses;
}

int TournamentPlayer::score() const
{
	return m_wins * 2 + m_draws;
}

void TournamentPlayer::addScore(Chess::Side side, int score)
{
	if (side == Chess::Side::NoSide)
		Q_UNREACHABLE();

	switch (score)
	{
	case 0:
		m_losses++;

		if (side == Chess::Side::White)
			m_whiteLosses++;
		break;
	case 1:
		m_draws++;

		if (side == Chess::Side::White)
			m_whiteDraws++;
		break;
	case 2:
		m_wins++;

		if (side == Chess::Side::White)
			m_whiteWins++;
		break;
	default:
		Q_UNREACHABLE();
		break;
	}
}

int TournamentPlayer::gamesFinished() const
{
	return m_wins + m_draws + m_losses;
}

void TournamentPlayer::addOutcome(int type, QString str)
{
	m_outcome[str]++;
	m_terminations[type]++;
}

int TournamentPlayer::outcomes(int type) const
{
	return m_terminations.at(type);
}

const QMap<QString, int> & TournamentPlayer::outcomeMap() const
{
	return m_outcome;
}

QJsonObject TournamentPlayer::toJson() const
{
    QJsonObject json;

    if (m_builder)
        json["builder"] = m_builder->toJson();
    
    json["timeControl"] = m_timeControl.toJson();
    
    if (m_book)
        json["book"] = m_book->toJson();

    json["bookDepth"] = m_bookDepth;
    json["wins"] = m_wins;
    json["draws"] = m_draws;
    json["losses"] = m_losses;
    json["whiteWins"] = m_whiteWins;
    json["whiteDraws"] = m_whiteDraws;
    json["whiteLosses"] = m_whiteLosses;

    QJsonArray terminationsArray;
    for (int val : m_terminations) {
        terminationsArray.append(val);
    }
    json["terminations"] = terminationsArray;

    QJsonObject outcomeObject;
    QMapIterator<QString, int> i(m_outcome);
    while (i.hasNext()) {
        i.next();
        outcomeObject.insert(i.key(), i.value());
    }
    json["outcomeMap"] = outcomeObject;

    return json;
}

#include "enginebuilder.h"
#include "engineconfiguration.h"

bool TournamentPlayer::loadFromJson(const QJsonObject& json)
{
    if (json.isEmpty())
        return false;

   if (json.contains("builder"))
    {
        if (!m_builder)
            m_builder = new EngineBuilder(EngineConfiguration());
            
        m_builder->loadFromJson(json["builder"].toObject());
    }

    if (json.contains("timeControl"))
        m_timeControl.loadFromJson(json["timeControl"].toObject());

    if (json.contains("book") && m_book)
        const_cast<OpeningBook*>(m_book)->loadFromJson(json["book"].toObject());

    m_bookDepth = json["bookDepth"].toInt();
    m_wins = json["wins"].toInt();
    m_draws = json["draws"].toInt();
    m_losses = json["losses"].toInt();
    m_whiteWins = json["whiteWins"].toInt();
    m_whiteDraws = json["whiteDraws"].toInt();
    m_whiteLosses = json["whiteLosses"].toInt();

    if (json.contains("terminations")) {
        m_terminations.clear();
        QJsonArray terminationsArray = json["terminations"].toArray();
        for (const QJsonValue& value : terminationsArray) {
            m_terminations.append(value.toInt());
        }
    }

    if (json.contains("outcomeMap")) {
        m_outcome.clear();
        QJsonObject outcomeObject = json["outcomeMap"].toObject();
        for (auto it = outcomeObject.begin(); it != outcomeObject.end(); ++it) {
            m_outcome.insert(it.key(), it.value().toInt());
        }
    }

    return true;
}