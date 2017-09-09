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
			std::vector<DebugQueryResult> *results_ptr;
            const std::set<std::string> *debug_enables_ptr;
			std::deque<std::string> current_path;
			std::deque<std::string> remaining_query;
			DebugQueryResult this_result;

			bool want_visit_children;
			bool want_report;
			bool set_report;
            bool want_control_debug_enable;

			Pimpl(std::vector<DebugQueryResult> &_results, const std::string &_query)
				: results_ptr(&_results)
                , debug_enables_ptr(nullptr)
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

            Pimpl(const std::set<std::string> &_debug_enables)
                : results_ptr(nullptr)
                , debug_enables_ptr(&_debug_enables)
                , current_path()
                , remaining_query()
            {
                SetupWantAndEmptyThisResult();
            }

            Pimpl(Pimpl &parent, const std::string &child_name)
				: results_ptr(parent.results_ptr)
                , debug_enables_ptr(parent.debug_enables_ptr)
				, current_path(parent.current_path)
				, remaining_query(parent.remaining_query)
			{
                if (results_ptr)
                {
                    // Ensure we have some query remaining and the child names
                    // matches the next part of the query

                    BBOX_ASSERT(!remaining_query.empty());
                    BBOX_ASSERT(remaining_query.front() == child_name);

                    current_path.push_back(std::move(remaining_query.front()));
                    remaining_query.pop_front();
                }
                else
                {
                    // Just update the current path

                    current_path.push_back(child_name);
                }

				SetupWantAndEmptyThisResult();
			}

			void SetupWantAndEmptyThisResult()
			{
				want_visit_children = true;
				want_report = results_ptr && remaining_query.empty();
				set_report = false;
                want_control_debug_enable = (debug_enables_ptr != nullptr);

				if (want_report)
				{
                    // If we're reporting, then we don't need
                    // to visit any more children

                    want_visit_children = false;

                    // Setup the result full path

					if (current_path.empty())
					{
						this_result.full_path = "/";
					}
					else
					{
						this_result.full_path = std::string("/") + boost::join(current_path, "/");
					}

                    // Determine the parent path

                    if (current_path.empty())
                    {
                        this_result.parent_path.clear();
                    }
                    else if (current_path.size() == 1)
                    {
                        this_result.parent_path = "/";
                    }
                    else
                    {
                        auto parent_path = current_path;

                        parent_path.pop_back();
                        
                        this_result.parent_path = std::string("/") + boost::join(parent_path, "/");
                    }
				}
			}

			~Pimpl()
			{
				if (want_report)
				{
					results_ptr->push_back(this_result);
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

        DebugVisitor::DebugVisitor(const std::set<std::string> &debug_enables)
            : m_pimpl(std::make_unique<Pimpl>(debug_enables))
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

        bool DebugVisitor::WantEnumerateChildren()
        {
            if (!m_pimpl)
                return false;

            return m_pimpl->want_report;
        }

		bool DebugVisitor::WantReport()
		{
			if (!m_pimpl)
				return false;

			return m_pimpl->want_report;
		}

        bool DebugVisitor::WantControlDebugEnable()
        {
            if (!m_pimpl)
                return false;

            return m_pimpl->want_control_debug_enable;
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
            else if (m_pimpl->debug_enables_ptr)
            {
                // When controlling debug enables we need to visit EVERY item

                result.m_pimpl = std::make_unique<Pimpl>(*m_pimpl, child_name);
            }
            else if (!m_pimpl->remaining_query.empty()
                && (child_name == m_pimpl->remaining_query.front()))
            {
                // When doing a query only return child visitor if the child
                // name exactly matches the next part of the remaining query

                result.m_pimpl = std::make_unique<Pimpl>(*m_pimpl, child_name);
            }

			return result;
		}

        void DebugVisitor::EnumerateChild(const std::string &name, std::string &&short_description)
        {
            BBOX_ASSERT(m_pimpl && m_pimpl->want_report);

            if (m_pimpl
                && m_pimpl->want_report)
            {
                std::deque<std::string> child_path(m_pimpl->current_path);
                child_path.push_back(name);

                m_pimpl->this_result.children.emplace_back(
                    name,
                    std::string("/") + boost::join(child_path, "/"),
                    std::move(short_description));
            }
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

        bool DebugVisitor::GetDebugEnableState()
        {
            BBOX_ASSERT(m_pimpl && m_pimpl->want_control_debug_enable);

            bool result = false;

            if (m_pimpl
                && m_pimpl->want_control_debug_enable)
            {
                std::string path = std::string("/") + boost::join(m_pimpl->current_path, "/");

                for (const std::string &enable : *m_pimpl->debug_enables_ptr)
                {
                    if ((path.size() >= enable.size())
                        && (path.substr(path.size() - enable.size()) == enable))
                    {
                        // This enable is a matching suffix for the current visitor path
                        result = true;
                    }
                }
            }

            return result;
        }

	} // namespace bbox::debug
} // namespace bbox
