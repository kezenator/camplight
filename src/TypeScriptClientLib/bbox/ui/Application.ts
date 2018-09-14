/// <reference path="./Container.ts"/>

namespace bbox.ui
{
    class BodyContainer extends Container
    {
        constructor()
        {
            super(window.document.body);
        }
    }

    export abstract class Application<StateType>
    {
        private m_type_prototype: bbox.enc.TypePrototype<StateType>;
        private m_body: Container;    

        constructor(state_prototype: bbox.enc.TypePrototype<StateType>)
        {
            this.m_type_prototype = state_prototype;

            (<any>window).bbox_ui_application = this;
            this.m_body = new BodyContainer(); 

            window.onload = (ev: Event) =>
            {
                var state: StateType = null;

                if (history.state
                    && (typeof (history.state) === "string"))
                {
                    state = this.xmlToState(history.state);
                }

                if (state === null)
                {
                    state = this.decodeUrlToState(window.location.pathname);
                }

                if (state === null)
                {
                    state = new this.m_type_prototype();
                }

                this.onLoad(state);
            };

            window.onpopstate = (ev: PopStateEvent) =>
            {
                var state: StateType = null;

                if (ev.state
                    && (typeof (ev.state) === "string"))
                {
                    state = this.xmlToState(ev.state);
                }

                if (state === null)
                {
                    state = this.decodeUrlToState(window.location.pathname);
                }

                if (state === null)
                {
                    state = new this.m_type_prototype();
                }

                this.onPopState(state);
            };
        }

        body(): Container
        {
            return this.m_body;
        }

        abstract onLoad(state: StateType): void;
        abstract onPopState(state: StateType): void;
        abstract decodeUrlToState(url: string): StateType;

        public pushState(url: string, state: StateType): void
        {
            var xml = this.stateToXml(state);

            history.pushState(xml, "", url);

            this.onPopState(state);
        }

        public replaceState(url: string, state: StateType): void
        {
            var xml = this.stateToXml(state);

            history.replaceState(xml, "", url);

            this.onPopState(state);
        }

        private stateToXml(state: StateType): string
        {
            if ((state === null)
                || (state === undefined))
            {
                return null;
            }

            var enc = new bbox.enc.ToXml("state");
            this.m_type_prototype.TYPE.toXml(state, enc);

            if (enc.hasError())
            {
                console.log("Could not encode state: " + enc.getErrorString());
                return null;
            }

            return enc.getXmlString();
        }

        private xmlToState(xml: string): StateType
        {
            if ((xml === null)
                || (xml === undefined)
                || (typeof (xml) != "string")
                || (xml == ""))
            {
                return null;
            }

            var enc = new bbox.enc.FromXml(xml, "state");

            var state: StateType = <StateType>this.m_type_prototype.TYPE.fromXml(enc);

            if (enc.hasError())
            {
                console.log("Could nto decode state: " + enc.getErrorString());
                return null;
            }

            return state;
        }
    }
}