namespace bbox.http.debug
{
    export class OutputView extends bbox.ui.Control
    {
        private app: Application;
        private div: HTMLDivElement;

        constructor(app: Application)
        {
            var div = document.createElement('div');

            super(div);

            this.app = app;
            this.div = div;

            this.div.classList.add("output-view");
        }

        public scrollToBottom(): void
        {
            this.div.scrollTop = this.div.scrollHeight;
        }

        public clear(): void
        {
            this.div.innerHTML = "";
        }

        public addMessage(message: string, is_error: boolean): void
        {
            var div = document.createElement('div');
            div.classList.add('output-view-item');

            var msg_div = document.createElement('div');
            msg_div.innerText = message;

            if (is_error)
                msg_div.classList.add('output-view-error');
            else
                msg_div.classList.add('output-view-message');

            div.appendChild(msg_div);
            this.div.appendChild(div);

            this.scrollToBottom();
        }

        public addDebugReport(report: msgs.DebugReportNotification): void
        {
            var div = document.createElement('div');
            div.classList.add('output-view-item');
            div.innerHTML = '<pre>' + report.contents_html + '</pre>';

            this.div.appendChild(div);

            this.scrollToBottom();
        }

        public addQueryResponse(entry: msgs.QueryResponseEntry): void
        {
            var div = document.createElement('div');
            div.classList.add('output-view-item');

            {
                var line = document.createElement("div");
                line.innerHTML = "Item: ";
                line.appendChild(this.createPathLink(entry.path, entry.path));
                div.appendChild(line);
            }

            if (entry.parent_path != "")
            {
                var line = document.createElement("div");
                line.innerHTML = "Parent: ";
                line.appendChild(this.createPathLink(entry.parent_path, entry.parent_path));
                div.appendChild(line);
            }

            for (var j = 0; j < entry.children.size(); ++j)
            {
                var child: msgs.QueryResponseChild = entry.children.at(j);

                var line = document.createElement("div");
                line.innerHTML = "Child: ";
                line.appendChild(this.createPathLink(child.name + " (" + child.short_description + ")", child.path));
                div.appendChild(line);
            }

            var content_div = document.createElement("div");
            content_div.innerHTML = "<pre>" + entry.report.contents_html + "</pre>";
            div.appendChild(content_div);

            this.div.appendChild(div);

            this.scrollToBottom();
        }

        private createPathLink(name: string, path: string): HTMLAnchorElement
        {
            var href = "/debug/state" + path;

            var link = document.createElement("a");
            link.href = href;
            link.innerText = name;

            link.onclick = (ev: MouseEvent) =>
            {
                ev.preventDefault();

                this.app.pushState_QueryState(path);
            };

            return link;
        }
    }
}