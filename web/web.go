package main

import "C"
import (
	"embed"
	"fmt"
	"html/template"
	"io"
	"net/http"
	"strings"

	"github.com/labstack/echo/v4"
	"github.com/tidwall/gjson"
)

//go:embed templates
var TEMPLATES_FOLDER embed.FS

// A custom html renderer, responsible for rendering html templates from an embedded file system in echo (or any web server)
type TemplateRenderer struct {
	templates *template.Template
}

func newRenderer(embeddedFolder *embed.FS, folderName string) (*TemplateRenderer, error) {
	templates, err := template.ParseFS(embeddedFolder, folderName+"/*.html")
	if err != nil {
		return nil, err
	}
	return &TemplateRenderer{templates}, nil
}
func (renderer *TemplateRenderer) Render(w io.Writer, name string, data interface{}, _ echo.Context) error {
	return renderer.templates.ExecuteTemplate(w, name, data)
}

// An interface returned by API functions / structs, with a single function, to convert the API result to viewable html
type HTMLRetriever interface {
	GetHTML() (rawHtml string, err error)
}

// The "HTMLFromJson" struct implements the "HTMLRetriever" interface, allowing html to be extracted from a json decoder, e.g. one obtained from an API response, as the name suggests
type HTMLRetrieverFromJson struct {
	apiError      error
	jsonString    string
	htmlFieldName string
}

func (htmlRetriever *HTMLRetrieverFromJson) GetHTML() (string, error) {
	if htmlRetriever.apiError != nil {
		return "", htmlRetriever.apiError
	}
	return gjson.Get(htmlRetriever.jsonString, htmlRetriever.htmlFieldName).String(), nil
}

// The "APIFetcher" struct allows us to fetch different routes we have defined with custom parameters, and get an http response
type APIFetcher struct {
	baseURL string
	routes  map[string]string
}

func (apiFetcher *APIFetcher) Get(routeName string, param string) (*http.Response, error) {
	fullUrl := apiFetcher.baseURL + fmt.Sprintf(apiFetcher.routes[routeName], param)
	return http.Get(fullUrl)
}

// The "HianimeHandler" struct allows us to fetch html results from the "hianime.to" anime streaming site
type HianimeHandler struct {
	hianimeApiFetcher *APIFetcher
}

func NewHianimeHandler(baseURL string) *HianimeHandler {
	return &HianimeHandler{
		&APIFetcher{
			baseURL,
			map[string]string{
				"ani-search":   "/search/suggest?keyword=%s",
				"ani-episodes": "/v2/episode/list/%s",
				"ani-servers":  "/v2/episode/servers?episodeId=%s",
			},
		},
	}
}
func (hianime *HianimeHandler) Get(routeName string, param string) HTMLRetriever {
	resp, err := hianime.hianimeApiFetcher.Get(routeName, param)
	if err != nil {
		return &HTMLRetrieverFromJson{apiError: err}
	}
	defer resp.Body.Close()
	respBytes, err := io.ReadAll(resp.Body)
	if err != nil {
		return &HTMLRetrieverFromJson{apiError: err}
	}
	return &HTMLRetrieverFromJson{
		nil,
		strings.ReplaceAll(string(respBytes), "data-src", "src"),
		"html",
	}
}

// Finally, we have a function to display html in a template
func createRouteToDisplayHtml(routeName string, paramName string, getHTMLRetriever func(routeName string, param string) HTMLRetriever) func(c echo.Context) error {
	return func(c echo.Context) error {
		htmlRetriever := getHTMLRetriever(routeName, c.Param(paramName))
		rawHtml, err := htmlRetriever.GetHTML()
		if err == nil {
			return c.Render(200, routeName, map[string]template.HTML{
				"raw_html": template.HTML(rawHtml),
			})
		}
		return c.String(500, err.Error())
	}
}

//export Serve
func Serve() {
	e := echo.New()
	renderer, err := newRenderer(&TEMPLATES_FOLDER, "templates")
	if err != nil {
		panic("could not load html template files: " + err.Error())
	}
	e.Renderer = renderer
	hianimeHandler := NewHianimeHandler("https://hianime.to/ajax")
	e.GET("/ani/search/:query", createRouteToDisplayHtml("ani-search", "query", hianimeHandler.Get))
	e.GET("/ani/episodes/:animeId", createRouteToDisplayHtml("ani-episodes", "animeId", hianimeHandler.Get))
	e.GET("/ani/servers/:episodeId", createRouteToDisplayHtml("ani-servers", "episodeId", hianimeHandler.Get))
	go e.Start(":33444")
}

func main() {}
