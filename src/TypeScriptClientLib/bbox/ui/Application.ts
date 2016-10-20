namespace bbox.ui
{
    class BodyContainer extends Container
    {
        constructor()
        {
            super(window.document.body);
        }
    }

    export abstract class Application
    {
        private m_body: Container;    

        constructor()
        {
            (<any>window).bbox_ui_application = this;
            this.m_body = new BodyContainer(); 

            window.onload = (ev: Event) =>
            {
                this.onload();
            };
        }

        body(): Container
        {
            return this.m_body;
        }

        abstract onload(): void;
    }
}