/**
 * @file
 *
 * Implementation file for the bbox::debug::DebugVisitor class.
 *
 */

#include <bbox/debug/DebugVisitor.h>
#include <bbox/debug/DebugQueryResult.h>
#include <bbox/Assert.h>

#include <boost/algorithm/string.hpp>
#include <deque>

namespace bbox
{
	namespace debug
	{

		struct DebugVisitor::Pimpl
		{
			std::vector<DebugQueryResult> &results;
			std::deque<std::string> current_path;
			std::deque<std::string> remaining_query;
			DebugQueryResult this_result;

			bool want_visit_children;
			bool want_report;
			bool set_report;

			Pimpl(std::vector<DebugQueryResult> &_results, const std::string &_query)
				: results(_results)
				, current_path()
				, remaining_query()
			{
				boost::split(remaining_query, _query, boost::is_any_of("/"));

				if (!remaining_query.empty()
					&& remaining_query.front().empty())
				{
					remaining_query.pop_front();
				}

				if (!remaining_query.empty()
					&& remaining_query.back().empty())
				{
					remaining_query.pop_back();
				}

				SetupWantAndEmptyThisResult();
			}

			Pimpl(Pimpl &parent, const std::string &child_name)
				: results(parent.results)
				, current_path(parent.current_path)
				, remaining_query(parent.remaining_query)
			{
				BBOX_ASSERT(!remaining_query.empty());
				BBOX_ASSERT(remaining_query.front() == child_name);

				current_path.push_back(std::move(remaining_query.front()));
				remaining_query.pop_front();

				SetupWantAndEmptyThisResult();
			}

			void SetupWantAndEmptyThisResult()
			{
				want_visit_children = true;
				want_report = remaining_query.empty();
				set_report = false;

				if (want_report)
				{
					// Setup the result full path

					if (current_path.empty())
					{
						this_result.full_path = "/";
					}
					else
					{
						this_result.full_path = std::string("/") + boost::join(current_path, "/");
					}
				}
			}

			~Pimpl()
			{
				if (want_report)
				{
					results.push_back(this_result);
				}
			}
		};

		DebugVisitor::DebugVisitor(std::unique_ptr<Pimpl> &&pimpl)
			: m_pimpl(std::move(pimpl))
		{
		}

		DebugVisitor::DebugVisitor(std::vector<DebugQueryResult> &results, const std::string &query)
			: m_pimpl(std::make_unique<Pimpl>(results, query))
		{
		}

		DebugVisitor::~DebugVisitor()
		{
		}

		bool DebugVisitor::WantVisitChildren()
		{
			if (!m_pimpl)
				return false;

			return m_pimpl->want_visit_children;
		}

		bool DebugVisitor::WantReport()
		{
			if (!m_pimpl)
				return false;

			return m_pimpl->want_report;
		}

		DebugVisitor DebugVisitor::ChildVisitor(const std::string &child_name)
		{
			BBOX_ASSERT(m_pimpl && m_pimpl->want_visit_children);

			DebugVisitor result{ std::unique_ptr<Pimpl>() };

			if (!m_pimpl
				|| !m_pimpl->want_visit_children)
			{
				// Invalid - don't return a valid visitor
			}
			else
			{
				// Always record this child

				if (m_pimpl->want_report)
				{
					m_pimpl->this_result.children.insert(child_name);
				}

				// Only return a real visitor for the actual
				// child that we want

				if (!m_pimpl->remaining_query.empty()
					&& (child_name == m_pimpl->remaining_query.front()))
				{
					result.m_pimpl = std::make_unique<Pimpl>(*m_pimpl, child_name);
				}
			}

			return result;
		}

		void DebugVisitor::SetReport(DebugReport report)
		{
			BBOX_ASSERT(m_pimpl && m_pimpl->want_report);

			if (m_pimpl
				&& m_pimpl->want_report)
			{
				BBOX_ASSERT(!m_pimpl->set_report);

				m_pimpl->this_result.report = std::move(report);
				m_pimpl->set_report = true;
			}
		}

	} // namespace bbox::debug
} // namespace bbox
