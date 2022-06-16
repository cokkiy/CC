using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.EntityFrameworkCore;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Logging;
using Serilog;
using LogViewerWeb.Data;
using LogViewerWeb.Data.DataPersisters;
using LogViewerWeb.Services;
using System.IO;
using System.Threading.Tasks;
using LogViewerWeb.RouteConstrants;

namespace LogViewerWeb
{
    public class Startup
    {
        public Startup(IConfiguration configuration)
        {
            Configuration = configuration;
        }

        public IConfiguration Configuration { get; }

        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services)
        {
            // Add framework services.
            services.AddDbContext<StationLogContext>(options =>
                    options.UseMySql(Configuration.GetConnectionString("StationLogConnectionString")));

            // Add data providers
            services.AddDataProviders();
            // Add data persisters
            services.AddDataPersisters();            

            // Add EMail and SMS Sender service
            services.AddTransient<IEmailSender, AuthMessageSender>();
            services.AddTransient<ISmsSender, AuthMessageSender>();            

            // Add TraceStateContainerService
            //services.AddSingleton<IStateContainerService, InMemoryStateContainerService>();

            // add cors policy
            services.AddCors(options =>
            {
                options.AddPolicy("Cors", policy =>
                {
                    policy.WithOrigins("*")
                    .AllowAnyHeader()
                    .AllowAnyMethod();
                });
            });

            // Add time type constraint
            services.AddRouting(options => { options.ConstraintMap.Add("TimeType", typeof(EnumConstrant<Models.ViewModels.TimeType>)); });

            // add mvc
            services.AddMvc();
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IHostingEnvironment env, ILoggerFactory loggerFactory)
        {  
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
                app.UseDatabaseErrorPage();
                app.UseBrowserLink();

                // Add Serilog log
                Log.Logger = new LoggerConfiguration()
                        .Enrich.FromLogContext()
                        .WriteTo.LiterateConsole() // add literate console for display
                        .WriteTo.RollingFile(@"./log/Trace.log.txt")
                        .CreateLogger();
            }
            else
            {
                app.UseExceptionHandler("/Home/Error");

                // Add Serilog log
                Log.Logger = new LoggerConfiguration()
                        .Enrich.FromLogContext()
                        .WriteTo.RollingFile(@"./log/Trace.log.txt")
                        .CreateLogger();
            }

            loggerFactory.AddSerilog();


            // use TraceStateContainer
            // app.UseInMemoryStateContainer();

            // use static file
            app.UseDefaultFiles();
            app.UseStaticFiles();

            // Use cors policy
            app.UseCors("Cors");           

            app.UseAuthentication();

            // Add external authentication middleware below. To configure them please see http://go.microsoft.com/fwlink/?LinkID=532715

            app.UseMvc(routes =>
            {
                routes.MapRoute(
                    name: "default",
                    template: "{controller=Home}/{action=Index}/{id?}");
            });

            app.Run(context =>
            {
                if(context.Request.Path.Value=="/login")
                {
                    //context.Response.Cookies.Append("r", "login");
                    string mainFile = Path.Combine(Directory.GetCurrentDirectory(), "wwwroot/index.html");
                    using (FileStream reader = new FileStream(mainFile, FileMode.Open, FileAccess.Read))
                    {
                        byte[] buffer = new byte[4096];
                        int len = reader.Read(buffer, 0, 4096);
                        context.Response.Body.Write(buffer, 0, len);
                        while (len == 4096)
                        {                            
                            len = reader.Read(buffer, 0, 4096);
                            if(len>0)
                                context.Response.Body.Write(buffer, 0, len);
                        }
                    }
                    return Task.CompletedTask;
                }
                else
                {
                    string redirectTo = string.Format("/?r={0}{1}", context.Request.Path.Value, context.Request.QueryString);
                    context.Response.Redirect(redirectTo);
                    return Task.FromResult(0);
                }
            });

        }
    }
}
