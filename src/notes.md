1. Input directory, output directory
2. Markdown templates in html


I need

List autogeneration from some token

```html
<bluesky-list directory="/posts" template="post.html" />
```

include items
```html
<bluesky-include path="/header.html"/>
```


Reverse includes

```html
<bluesky-template path="/frame.html">
...
</bluesky-template>
```

```html
<html>
  <head>
  ...
  </head>
  <body>
    <bluesky-placeholder />
  </body>
</html>
```
