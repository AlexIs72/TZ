using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.AspNetCore.Http;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using TranslatorApp.Data;

namespace TranslatorApp
{
    public class Startup
    {
        public Startup()
        {
            using (var client = new LangDbContext())
            {
                //Create the database file at a path defined in SimpleDataStorage
                client.Database.EnsureCreated();
                //Create the database tables defined in SimpleDataStorage
                //client.Database.Migrate();
            }
        }

        // This method gets called by the runtime. Use this method to add services to the container.
        // For more information on how to configure your application, visit https://go.microsoft.com/fwlink/?LinkID=398940
        public void ConfigureServices(IServiceCollection services)
        {
            services.AddSession();
            services.AddMvc().SetCompatibilityVersion(CompatibilityVersion.Version_2_1);
            services.AddEntityFrameworkSqlite().AddDbContext<LangDbContext>();
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IHostingEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }
            /*else
            {
                app.UseExceptionHandler("/Translator/Error");
            }*/

            app.UseSession();
            app.UseStaticFiles();
            app.UseCookiePolicy();

            app.UseMvc(routes =>
            {
                routes.MapRoute(
                    name: "default",
                    template: "{controller=Translator}/{action=Index}");
            });


        }
    }
}
