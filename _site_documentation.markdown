### Testing locally:

Install links [here](https://jekyllrb.com/docs/installation/).

I just let jekyll auto-generate the files it needs, I recommend trying this on an empty folder first,
you can just copy over whatever it makes too.

```
# Move to parent dir
cd ..

# make sure your markdown file is named index.md, then do:
jekyll new <your repo name> --force
```

Above will generate all the files Jekyll needs to enable local testing.

Then run with:

```
bundle exec jekyll serve
```

Theme info here:
https://github.com/pages-themes/midnight

Note: This file won't get built, jekyll ignores files starting with "_".