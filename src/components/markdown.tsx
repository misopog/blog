import ReactMarkdown from 'react-markdown'
import remarkGfm from 'remark-gfm'

interface MarkdownProps {
  children: string
}

export function Markdown({ children }: MarkdownProps) {
  return (
    <ReactMarkdown
      remarkPlugins={[remarkGfm]}
      components={{
        h1: ({ children }) => (
          <h1 className="text-3xl font-bold mt-8 mb-4">{children}</h1>
        ),
        h2: ({ children }) => (
          <h2 className="text-2xl font-bold mt-6 mb-3">{children}</h2>
        ),
        p: ({ children }) => (
          <p className="mb-4 leading-relaxed">{children}</p>
        ),
        a: ({ href, children }) => (
          <a href={href} className="text-blue-400 hover:underline">
            {children}
          </a>
        ),
        ul: ({ children }) => (
          <ul className="list-disc pl-6 mb-4">{children}</ul>
        ),
        ol: ({ children }) => (
          <ol className="list-decimal pl-6 mb-4">{children}</ol>
        ),
        code: ({ children }) => (
          <code className="bg-white/10 rounded px-1.5 py-0.5">{children}</code>
        ),
        pre: ({ children }) => (
          <pre className="bg-white/10 p-4 rounded-lg overflow-x-auto mb-4">
            {children}
          </pre>
        ),
      }}
    >
      {children}
    </ReactMarkdown>
  )
}
